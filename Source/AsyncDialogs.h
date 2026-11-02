#pragma once

#include <JuceHeader.h>

/*
    Small async wrappers around the dialogs this app needs.

    Everything here returns immediately and reports the user's answer through a
    callback, so the app never needs JUCE_MODAL_LOOPS_PERMITTED. Callbacks only
    fire when the user confirms unless stated otherwise, and always on the
    message thread.
*/

/** A message with a single "ok" button. Nothing to report back. */
void showMessageAsync (MessageBoxIconType icon, const String& title, const String& message);

/** A two button confirmation. `callback` is called with true only if the user
    picked the first (affirmative) button.
*/
void showConfirmAsync (MessageBoxIconType icon, const String& title, const String& message,
                       const String& okText, const String& cancelText,
                       std::function<void (bool)> callback);

/** An ok/cancel prompt that can carry a combo box of choices and/or a text field.

    Pass an empty `choices` to omit the combo box, or an empty `textLabel` to omit
    the text editor. `onOk` runs only if the user clicks ok, and receives the id of
    the selected combo item (0 when there is no combo box) and the trimmed text
    field contents (empty when there is no text field).

    When a text field is requested, `onOk` is not called if it was left empty —
    that matches what every caller here wants and keeps the call sites clean.
*/
void showChooserAsync (const String& message,
                       const StringArray& choices, const Array<int>& choiceIds,
                       const String& textLabel,
                       std::function<void (int selectedId, String text)> onOk);
