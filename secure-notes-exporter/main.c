//
//  main.c
//  secure-notes-exporter
//
//  Created by Greg Hurrell on 5/9/14.
//  Copyright (c) 2014 Greg Hurrell. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>

void printItem(const void *value, void *context) {
    CFDictionaryRef dictionary = value;
    CFNumberRef itemType = CFDictionaryGetValue(dictionary, kSecAttrType);
    CFNumberRef noteType = (CFNumberRef)context;
    if (!itemType || !CFEqual(itemType, noteType)) {
        // not a note
        return;
    }

    CFShow(noteType);
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
    CFArrayRef items = nil;
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

