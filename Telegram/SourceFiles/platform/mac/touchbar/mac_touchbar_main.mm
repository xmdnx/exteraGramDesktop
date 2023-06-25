/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#include "platform/mac/touchbar/mac_touchbar_main.h"

#include "platform/mac/touchbar/items/mac_formatter_item.h"
#include "platform/mac/touchbar/items/mac_pinned_chats_item.h"
#include "platform/mac/touchbar/items/mac_scrubber_item.h"
#include "platform/mac/touchbar/mac_touchbar_common.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#import <AppKit/NSCustomTouchBarItem.h>

using namespace TouchBar::Main;

#pragma mark - TouchBarMain

@interface TouchBarMain()
@end // @interface TouchBarMain

@implementation TouchBarMain

- (id)init:(not_null<Window::Controller*>)controller
		touchBarSwitches:(rpl::producer<>)touchBarSwitches {
	self = [super init];
	if (!self) {
		return self;
	}

	auto *pin = [[[NSCustomTouchBarItem alloc]
		initWithIdentifier:kPinnedPanelItemIdentifier] autorelease];
	pin.view = [[[PinnedDialogsPanel alloc]
		init:(&controller->sessionController()->session())
		destroyEvent:std::move(touchBarSwitches)] autorelease];

	auto *sticker = [[[StickerEmojiPopover alloc]
		init:controller
		identifier:kPopoverPickerItemIdentifier] autorelease];

	auto *format = [[[TextFormatPopover alloc]
		init:kPopoverInputItemIdentifier] autorelease];

	self.templateItems = [NSSet setWithArray:@[pin, sticker, format]];

	return self;
}

@end // @implementation TouchBarMain
