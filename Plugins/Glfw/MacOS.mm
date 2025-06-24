#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

void* get_metal_layer(void* window) {
    NSWindow* nsWindow = (NSWindow*)window;
    NSView* nsView = [nsWindow contentView];
    [nsView setWantsLayer:YES];

    // Set CAMetalLayer as the backing layer if it's not already
    if (![nsView.layer isKindOfClass:[CAMetalLayer class]]) {
        CAMetalLayer* metalLayer = [CAMetalLayer layer];
        nsView.layer = metalLayer;
    }

    return (CAMetalLayer*)nsView.layer;;
}
