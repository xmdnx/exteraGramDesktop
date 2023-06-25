/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/flags.h"
#include "boxes/abstract_box.h"
#include "ui/chat/attach/attach_prepare.h"
#include "ui/chat/attach/attach_send_files_way.h"
#include "ui/widgets/popup_menu.h"
#include "storage/localimageloader.h"
#include "storage/storage_media_prepare.h"

namespace Window {
class SessionController;
} // namespace Window

namespace Api {
struct SendOptions;
enum class SendType;
} // namespace Api

namespace ChatHelpers {
class TabbedPanel;
} // namespace ChatHelpers

namespace Ui {
class Checkbox;
class RoundButton;
class InputField;
struct GroupMediaLayout;
class EmojiButton;
class AlbumPreview;
class VerticalLayout;
class FlatLabel;
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace SendMenu {
enum class Type;
} // namespace SendMenu

enum class SendFilesAllow {
	OnlyOne = (1 << 0),
	Photos = (1 << 1),
	Videos = (1 << 2),
	Music = (1 << 3),
	Files = (1 << 4),
	Stickers = (1 << 5),
	Gifs = (1 << 6),
	EmojiWithoutPremium = (1 << 7),
	Texts = (1 << 8),
};
inline constexpr bool is_flag_type(SendFilesAllow) { return true; }
using SendFilesLimits = base::flags<SendFilesAllow>;

using SendFilesCheck = Fn<bool(
	const Ui::PreparedFile &file,
	bool compress,
	bool silent)>;

[[nodiscard]] SendFilesLimits DefaultLimitsForPeer(not_null<PeerData*> peer);
[[nodiscard]] SendFilesCheck DefaultCheckForPeer(
	not_null<Window::SessionController*> controller,
	not_null<PeerData*> peer);

class SendFilesBox : public Ui::BoxContent {
public:
	enum class SendLimit {
		One,
		Many
	};
	SendFilesBox(
		QWidget*,
		not_null<Window::SessionController*> controller,
		Ui::PreparedList &&list,
		const TextWithTags &caption,
		SendFilesLimits limits,
		SendFilesCheck check,
		Api::SendType sendType,
		SendMenu::Type sendMenuType);

	void setConfirmedCallback(
		Fn<void(
			Ui::PreparedList &&list,
			Ui::SendFilesWay way,
			TextWithTags &&caption,
			Api::SendOptions options,
			bool ctrlShiftEnter)> callback) {
		_confirmedCallback = std::move(callback);
	}
	void setCancelledCallback(Fn<void()> callback) {
		_cancelledCallback = std::move(callback);
	}

	~SendFilesBox();

protected:
	void prepare() override;
	void setInnerFocus() override;

	void keyPressEvent(QKeyEvent *e) override;
	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

private:
	class Block final {
	public:
		Block(
			not_null<QWidget*> parent,
			not_null<std::vector<Ui::PreparedFile>*> items,
			int from,
			int till,
			Fn<bool()> gifPaused,
			Ui::SendFilesWay way);
		Block(Block &&other) = default;
		Block &operator=(Block &&other) = default;

		[[nodiscard]] int fromIndex() const;
		[[nodiscard]] int tillIndex() const;
		[[nodiscard]] object_ptr<Ui::RpWidget> takeWidget();

		[[nodiscard]] rpl::producer<int> itemDeleteRequest() const;
		[[nodiscard]] rpl::producer<int> itemReplaceRequest() const;
		[[nodiscard]] rpl::producer<int> itemModifyRequest() const;

		void setSendWay(Ui::SendFilesWay way);
		void toggleSpoilers(bool enabled);
		void applyChanges();

	private:
		base::unique_qptr<Ui::RpWidget> _preview;
		not_null<std::vector<Ui::PreparedFile>*> _items;
		int _from = 0;
		int _till = 0;
		bool _isAlbum = false;
		bool _isSingleMedia = false;

	};

	void initSendWay();
	void initPreview();
	[[nodiscard]] bool hasSendMenu() const;
	[[nodiscard]] bool hasSpoilerMenu() const;
	[[nodiscard]] bool allWithSpoilers();
	[[nodiscard]] bool checkWithWay(
		Ui::SendFilesWay way,
		bool silent = false) const;
	[[nodiscard]] bool checkWith(
		const Ui::PreparedList &added,
		Ui::SendFilesWay way,
		bool silent = false) const;
	void addMenuButton();
	void applyBlockChanges();
	void toggleSpoilers(bool enabled);

	bool validateLength(const QString &text) const;
	void refreshButtons();
	void refreshControls(bool initial = false);
	void setupSendWayControls();
	void setupCaption();

	void setupEmojiPanel();
	void updateSendWayControls();
	void updateEmojiPanelGeometry();
	void emojiFilterForGeometry(not_null<QEvent*> event);

	void preparePreview();
	void generatePreviewFrom(int fromBlock);

	void send(Api::SendOptions options, bool ctrlShiftEnter = false);
	void sendSilent();
	void sendScheduled();
	void sendWhenOnline();
	void captionResized();
	void saveSendWaySettings();

	void setupDragArea();
	void refreshTitleText();
	void updateBoxSize();
	void updateControlsGeometry();
	void updateCaptionPlaceholder();

	bool canAddFiles(not_null<const QMimeData*> data) const;
	bool addFiles(not_null<const QMimeData*> data);
	bool addFiles(Ui::PreparedList list);
	void addFile(Ui::PreparedFile &&file);
	void pushBlock(int from, int till);

	void openDialogToAddFileToAlbum();
	void refreshAllAfterChanges(int fromItem, Fn<void()> perform = nullptr);

	void enqueueNextPrepare();
	void addPreparedAsyncFile(Ui::PreparedFile &&file);

	const not_null<Window::SessionController*> _controller;
	const Api::SendType _sendType = Api::SendType();

	QString _titleText;
	rpl::variable<int> _titleHeight = 0;

	Ui::PreparedList _list;
	std::optional<int> _removingIndex;

	SendFilesLimits _limits = {};
	SendMenu::Type _sendMenuType = SendMenu::Type();

	SendFilesCheck _check;
	Fn<void(
		Ui::PreparedList &&list,
		Ui::SendFilesWay way,
		TextWithTags &&caption,
		Api::SendOptions options,
		bool ctrlShiftEnter)> _confirmedCallback;
	Fn<void()> _cancelledCallback;
	bool _confirmed = false;

	object_ptr<Ui::InputField> _caption = { nullptr };
	TextWithTags _prefilledCaptionText;
	object_ptr<Ui::EmojiButton> _emojiToggle = { nullptr };
	base::unique_qptr<ChatHelpers::TabbedPanel> _emojiPanel;
	base::unique_qptr<QObject> _emojiFilter;

	object_ptr<Ui::Checkbox> _groupFiles = { nullptr };
	object_ptr<Ui::Checkbox> _sendImagesAsPhotos = { nullptr };
	object_ptr<Ui::Checkbox> _wayRemember = { nullptr };
	object_ptr<Ui::FlatLabel> _hintLabel = { nullptr };
	rpl::variable<Ui::SendFilesWay> _sendWay = Ui::SendFilesWay();

	rpl::variable<int> _footerHeight = 0;
	rpl::lifetime _dimensionsLifetime;

	object_ptr<Ui::ScrollArea> _scroll;
	QPointer<Ui::VerticalLayout> _inner;
	std::vector<Block> _blocks;
	Fn<void()> _whenReadySend;
	bool _preparing = false;

	base::unique_qptr<Ui::PopupMenu> _menu;

	QPointer<Ui::RoundButton> _send;
	QPointer<Ui::RoundButton> _addFile;

};
