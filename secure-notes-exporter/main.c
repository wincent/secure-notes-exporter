//
//  main.c
//  secure-notes-exporter
//
//  Created by Greg Hurrell on 5/9/14.
//  Copyright (c) 2014 Greg Hurrell. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>

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
    CFShow(items);
    CFRelease(items);
    return 0;
}

