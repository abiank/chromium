// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_TEST_AUTOMATION_AUTOMATION_JSON_REQUESTS_H_
#define CHROME_TEST_AUTOMATION_AUTOMATION_JSON_REQUESTS_H_
#pragma once

#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "base/file_path.h"
#include "base/string_number_conversions.h"
#include "chrome/common/automation_constants.h"
#include "chrome/common/automation_id.h"
#include "ui/base/keycodes/keyboard_codes.h"

class AutomationMessageSender;
class FilePath;

namespace base {
class DictionaryValue;
class ListValue;
class Value;
}

struct WebKeyEvent {
  WebKeyEvent(automation::KeyEventTypes type,
              ui::KeyboardCode key_code,
              const std::string& unmodified_text,
              const std::string& modified_text,
              int modifiers);

  automation::KeyEventTypes type;
  ui::KeyboardCode key_code;
  std::string unmodified_text;
  std::string modified_text;
  int modifiers;
};

// Uniquely identifies a particular WebView.
// This is needed because Chrome used to accept just tab IDs, while
// now it accepts IDs for other types of WebViews.
// TOOD(kkania): Remove this abstraction once Chrome 16 is unsupported.
class WebViewId {
 public:
  // Creates an ID for the given view ID.
  static WebViewId ForView(const AutomationId& view_id);

  // Creates an ID for the given tab ID.
  static WebViewId ForOldStyleTab(int tab_id);

  // Creates an invalid ID.
  WebViewId();

  // Updates the given dictionary to include this ID. If the ID refers to a
  // view ID, |view_id_key| will be the key modified in the dictionary.
  void UpdateDictionary(base::DictionaryValue* dictionary,
                        const std::string& view_id_key) const;

  // Returns whether this ID is valid. Even if it is valid, the object it
  // refers to may not exist.
  bool IsValid() const;

  // Returns an |AutomationId| made from this ID.
  AutomationId GetId() const;

  // Returns whether this ID refers to a tab.
  bool IsTab() const;

  int tab_id() const;

  // The old style is to use a single integer ID for a tab. The new style is
  // to use an automation ID which may refer to a number of different object
  // types.
  bool old_style() const;

 private:
  // Whether this ID is an old-style integer tab ID.
  bool old_style_;

  AutomationId id_;
  int tab_id_;
};

// Used to locate a WebView. The same locator may locate different WebViews
// at different times. This is needed because Chrome used to only accept
// browser/tab indices, while the new Chrome accepts a unique ID.
// TOOD(kkania): Simplify this once Chrome 16 is unsupported.
class WebViewLocator {
 public:
  // Creates a locator for locating the given tab.
  static WebViewLocator ForIndexPair(int browser_index, int tab_index);

  // Creates a locator for locating the given view.
  static WebViewLocator ForViewId(const AutomationId& view_id);

  // Creates an invalid locator.
  WebViewLocator();
  ~WebViewLocator();

  // Updates the given dictionary to include the given locator information.
  // If this locator is a view ID, |view_id_key| will be the name of the key
  // to update.
  void UpdateDictionary(base::DictionaryValue* dict,
                        const std::string& view_id_key) const;

  int browser_index() const;
  int tab_index() const;

 private:
  enum Type {
    kTypeIndexPair,
    kTypeViewId,
  };

  struct IndexPair {
    int browser_index;
    int tab_index;
  };

  struct Locator {
    Locator();
    ~Locator();

    IndexPair index_pair;
    AutomationId view_id;
  };

  Type type_;
  Locator locator_;
};

// Collection of info about a given WebView.
struct WebViewInfo {
  WebViewInfo(const WebViewId& view_id,
              const std::string& extension_id);
  ~WebViewInfo();

  // The view's unique ID.
  WebViewId view_id;

  // If this view belongs to an extension, this ID will be set to it.
  std::string extension_id;
};

// Sends a JSON request to the chrome automation provider. Returns true
// if the JSON request was successfully sent and the reply was received.
// If true, |success| will be set to whether the JSON request was
// completed successfully by the automation provider.
bool SendAutomationJSONRequest(AutomationMessageSender* sender,
                               const std::string& request,
                               int timeout_ms,
                               std::string* reply,
                               bool* success) WARN_UNUSED_RESULT;

// Same as above, but uses the given |AutomationMessageSender|'s default timeout
// value.
bool SendAutomationJSONRequestWithDefaultTimeout(
    AutomationMessageSender* sender,
    const std::string& request,
    std::string* reply,
    bool* success);

// Requests the current browser and tab indices for the given tab ID.
// Returns true on success.
bool SendGetIndicesFromTabIdJSONRequest(
    AutomationMessageSender* sender,
    int tab_id,
    int* browser_index,
    int* tab_index,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the current browser and tab indices for the given |TabProxy|
// handle. Returns true on success.
bool SendGetIndicesFromTabHandleJSONRequest(
    AutomationMessageSender* sender,
    int tab_proxy_handle,
    int* browser_index,
    int* tab_index,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to navigate to the given url and wait for the given number of
// navigations to complete. Returns true on success.
bool SendNavigateToURLJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    const std::string& url,
    int navigation_count,
    AutomationMsg_NavigationResponseValues* nav_response,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the given javascript to be executed in the frame specified by the
// given xpath. Returns true on success. If true, |result| will be set to the
// result of the execution and ownership will be given to the caller.
bool SendExecuteJavascriptJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    const std::string& frame_xpath,
    const std::string& javascript,
    base::Value** result,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the specified view to go forward. Waits for the load to complete.
// Returns true on success.
bool SendGoForwardJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the specified view to go back. Waits for the load to complete.
// Returns true on success.
bool SendGoBackJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the specified view to reload. Waits for the load to complete.
// Returns true on success.
bool SendReloadJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests a snapshot of the entire page to be saved to the given path
// in PNG format.
// Returns true on success.
bool SendCaptureEntirePageJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    const FilePath& path,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests all the cookies for the given URL. On success returns true and
// caller takes ownership of |cookies|, which is a list of all the cookies in
// dictionary format.
bool SendGetCookiesJSONRequest(
    AutomationMessageSender* sender,
    const std::string& url,
    base::ListValue** cookies,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests deletion of the cookie with the given name and URL. Returns true
// on success.
bool SendDeleteCookieJSONRequest(
    AutomationMessageSender* sender,
    const std::string& url,
    const std::string& cookie_name,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests setting the given cookie for the given URL. Returns true on
// success. The caller retains ownership of |cookie_dict|.
bool SendSetCookieJSONRequest(
    AutomationMessageSender* sender,
    const std::string& url,
    base::DictionaryValue* cookie_dict,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the IDs for all open tabs. Returns true on success.
bool SendGetTabIdsJSONRequest(
    AutomationMessageSender* sender,
    std::vector<WebViewInfo>* views,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests info for all open views. Returns true on success.
bool SendGetWebViewsJSONRequest(
    AutomationMessageSender* sender,
    std::vector<WebViewInfo>* views,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests whether the given tab ID is valid. Returns true on success.
bool SendIsTabIdValidJSONRequest(
    AutomationMessageSender* sender,
    const WebViewId& view_id,
    bool* is_valid,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests whether the given automation ID refers to an actual automation
// object. Returns true on success.
bool SendDoesAutomationObjectExistJSONRequest(
    AutomationMessageSender* sender,
    const WebViewId& view_id,
    bool* does_exist,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to close the given view. Returns true on success.
bool SendCloseViewJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the WebKit event for a mouse move to the given
// coordinate in the specified view. Returns true on success.
bool SendMouseMoveJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    int x,
    int y,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the WebKit events for a mouse click at the given
// coordinate in the specified view. Returns true on success.
bool SendMouseClickJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    automation::MouseButton button,
    int x,
    int y,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the WebKit events for a mouse drag from the start to end
// coordinates given in the specified view. Returns true on success.
bool SendMouseDragJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    int start_x,
    int start_y,
    int end_x,
    int end_y,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the WebKit event for a mouse button down at the given
// coordinate in the specified view. Returns true on success.
bool SendMouseButtonDownJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    int x,
    int y,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the WebKit event for a mouse button up at the given
// coordinate in the specified view. Returns true on success.
bool SendMouseButtonUpJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    int x,
    int y,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the WebKit event for a mouse double click at the given
// coordinate in the specified view. Returns true on success.
bool SendMouseDoubleClickJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    int x,
    int y,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the WebKit event for the given |WebKeyEvent| in a
// specified view. Returns true on success.
bool SendWebKeyEventJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    const WebKeyEvent& key_event,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to send the key event for the given keycode+modifiers to a
// browser window containing the specified view. Returns true on success.
bool SendNativeKeyEventJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    ui::KeyboardCode key_code,
    int modifiers,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to drag and drop the file paths at the given coordinate in the
// specified view. Returns true on success.
bool SendDragAndDropFilePathsJSONRequest(
    AutomationMessageSender* sender,
    const WebViewLocator& locator,
    int x,
    int y,
    const std::vector<FilePath::StringType>& paths,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to get the active JavaScript modal dialog's message. Returns true
// on success.
bool SendGetAppModalDialogMessageJSONRequest(
    AutomationMessageSender* sender,
    std::string* message,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to accept or dismiss the active JavaScript modal dialog.
// Returns true on success.
bool SendAcceptOrDismissAppModalDialogJSONRequest(
    AutomationMessageSender* sender,
    bool accept,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to accept the active JavaScript modal dialog with the given prompt
// text. Returns true on success.
bool SendAcceptPromptAppModalDialogJSONRequest(
    AutomationMessageSender* sender,
    const std::string& prompt_text,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests to wait for all views to stop loading. Returns true on success.
bool SendWaitForAllViewsToStopLoadingJSONRequest(
    AutomationMessageSender* sender,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the version of ChromeDriver automation supported by the automation
// server. Returns true on success.
bool SendGetChromeDriverAutomationVersion(
    AutomationMessageSender* sender,
    int* version,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests that the given extension be installed. If |with_ui| is false,
// the extension will be installed silently. Returns true on success.
bool SendInstallExtensionJSONRequest(
    AutomationMessageSender* sender,
    const FilePath& path,
    bool with_ui,
    std::string* extension_id,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests info about all installed extensions. Returns true on success.
bool SendGetExtensionsInfoJSONRequest(
    AutomationMessageSender* sender,
    base::ListValue* extensions_list,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests whether the given extension's page action is visible in the
// given tab.
bool SendIsPageActionVisibleJSONRequest(
    AutomationMessageSender* sender,
    const WebViewId& tab_id,
    const std::string& extension_id,
    bool* is_visible,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests a modification of the given extension state. Returns true on
// success.
bool SendSetExtensionStateJSONRequest(
    AutomationMessageSender* sender,
    const std::string& extension_id,
    bool enable,
    bool allow_in_incognito,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the given extension's action button be pressed. Returns true on
// success.
bool SendClickExtensionButtonJSONRequest(
    AutomationMessageSender* sender,
    const std::string& extension_id,
    bool browser_action,
    std::string* error_msg) WARN_UNUSED_RESULT;

// Requests the given extension be uninstalled. Returns true on success.
bool SendUninstallExtensionJSONRequest(
    AutomationMessageSender* sender,
    const std::string& extension_id,
    std::string* error_msg) WARN_UNUSED_RESULT;

#endif  // CHROME_TEST_AUTOMATION_AUTOMATION_JSON_REQUESTS_H_
