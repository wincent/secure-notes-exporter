//
//  main.m
//  secure-notes-exporter
//
//  Created by Greg Hurrell on 5/9/14.
//  Copyright (c) 2014 Greg Hurrell. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>

void printItem(const void *value, void *context) {
    CFDictionaryRef item = value;
    CFNumberRef itemType = CFDictionaryGetValue(item, kSecAttrType);
    CFNumberRef noteType = (CFNumberRef)context;
    if (!itemType || !CFEqual(itemType, noteType)) {
        // not a note
        return;
    }

    CFStringRef serviceName = CFDictionaryGetValue(item, kSecAttrService);
    assert(serviceName);
    char *serviceNamePtr = (char *)CFStringGetCStringPtr(serviceName, kCFStringEncodingUTF8);
    if (!serviceNamePtr) {
        CFIndex serviceNameLength = CFStringGetLength(serviceName);
        CFIndex maxServiceNameLength = CFStringGetMaximumSizeForEncoding(serviceNameLength, kCFStringEncodingUTF8);
        serviceNamePtr = malloc(maxServiceNameLength);
        assert(serviceNamePtr);
        assert(CFStringGetCString(serviceName, serviceNamePtr, maxServiceNameLength, kCFStringEncodingUTF8));
    }

    UInt32 passwordLength;
    void *passwordData;
    OSStatus status = SecKeychainFindGenericPassword(
        NULL,
        (UInt32)strlen(serviceNamePtr),
        serviceNamePtr,
        0,
        NULL,
        &passwordLength,
        &passwordData,
        NULL
    );
    assert(status == 0);

    CFDataRef data = CFDataCreate(kCFAllocatorDefault, passwordData, passwordLength);
    CFPropertyListRef propertyList = CFPropertyListCreateWithData(
        kCFAllocatorDefault, data, kCFPropertyListImmutable, NULL, NULL
    );
    CFRelease(data);

    if (!propertyList) {
        printf("error\n");
    } else {
        CFShow(CFDictionaryGetValue(propertyList, CFSTR("NOTE")));
        CFRelease(propertyList);
    }
}

int main(int argc, const char * argv[])
{
    // create query
    CFStringRef keys[] = { kSecReturnAttributes, kSecMatchLimit, kSecClass };
    CFTypeRef values[] = { kCFBooleanTrue, kSecMatchLimitAll, kSecClassGenericPassword };
    CFDictionaryRef query = CFDictionaryCreate(
        kCFAllocatorDefault,
        (const void **)keys,
        (const void **)values,
        3,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks
    );

    // get search results
    CFArrayRef items = NULL;
    OSStatus status = SecItemCopyMatching(query, (CFTypeRef *)&items);
    CFRelease(query);
    assert(status == 0);

    // iterate over "Secure Note" items
    CFRange range = CFRangeMake(0, CFArrayGetCount(items));
    SInt32 note = 'note';
    CFNumberRef noteRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &note);
    CFArrayApplyFunction(items, range, printItem, (void *)noteRef);
    CFRelease(noteRef);
    CFRelease(items);
    return 0;
}

