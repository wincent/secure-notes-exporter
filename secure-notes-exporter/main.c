//
//  main.m
//  secure-notes-exporter
//
//  Created by Greg Hurrell on 5/9/14.
//  Copyright (c) 2014 Greg Hurrell. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>

CFStringRef createEscapedString(CFStringRef string) {
    CFMutableStringRef mutable = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, string);
    CFStringFindAndReplace(
        mutable,
        CFSTR("\""),
        CFSTR("\"\""),
        CFRangeMake(0, CFStringGetLength(mutable)),
        0
    );
    return mutable;
}

const char *createCStringFromString(CFStringRef string) {
    CFIndex stringLength = CFStringGetLength(string);
    CFIndex maxStringLength = CFStringGetMaximumSizeForEncoding(stringLength, kCFStringEncodingUTF8);
    const char *ptr = malloc(maxStringLength);
    assert(ptr);
    assert(CFStringGetCString(string, (char *)ptr, maxStringLength, kCFStringEncodingUTF8));
    return ptr;
}

void printHeader(void) {
    printf("url,name,extra,group\n");
}

void printCSV(CFStringRef serviceName, CFStringRef note) {
    const char *quote = "\"";
    CFStringRef escapedService = createEscapedString(serviceName);
    CFStringRef escapedNote = createEscapedString(note);
    const char *name = createCStringFromString(escapedService);
    const char *extra = createCStringFromString(escapedNote);

    printf(
        "http://sn,"
        "%s%s%s,"
        "%s%s%s,"
        "%sSecure Notes%s\n",
        quote, name, quote,
        quote, extra, quote,
        quote, quote
    );
    CFRelease(escapedService);
    CFRelease(escapedNote);
    free((void *)name);
    free((void *)extra);
}

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
    const char *serviceNamePtr = createCStringFromString(serviceName);

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
    free((void *)serviceNamePtr);

    CFDataRef data = CFDataCreate(kCFAllocatorDefault, passwordData, passwordLength);
    CFPropertyListRef propertyList = CFPropertyListCreateWithData(
        kCFAllocatorDefault, data, kCFPropertyListImmutable, NULL, NULL
    );
    CFRelease(data);

    CFStringRef note;
    if (!propertyList) {
        fprintf(stderr, "ERROR: failed to load property list\n");
        return;
    } else if (CFGetTypeID(propertyList) == CFDictionaryGetTypeID()) {
        note = CFDictionaryGetValue(propertyList, CFSTR("NOTE"));
    } else if (CFGetTypeID(propertyList) == CFStringGetTypeID()) {
        note = propertyList;
    } else {
        fprintf(stderr, "ERROR: not expected type\n");
        return;
    }
    printCSV(serviceName, note);
    CFRelease(propertyList);
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
    printHeader();
    CFArrayApplyFunction(items, range, printItem, (void *)noteRef);
    CFRelease(noteRef);
    CFRelease(items);
    return 0;
}

