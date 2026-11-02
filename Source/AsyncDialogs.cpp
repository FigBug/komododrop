#include "AsyncDialogs.h"

static const char* dialogTitle = "Komodo Drop";

void showMessageAsync (MessageBoxIconType icon, const String& title, const String& message)
{
    AlertWindow::showAsync (MessageBoxOptions()
                                .withIconType (icon)
                                .withTitle (title)
                                .withMessage (message)
                                .withButton ("ok"),
                            nullptr);
}

void showConfirmAsync (MessageBoxIconType icon, const String& title, const String& message,
                       const String& okText, const String& cancelText,
                       std::function<void (bool)> callback)
{
    AlertWindow::showAsync (MessageBoxOptions()
                                .withIconType (icon)
                                .withTitle (title)
                                .withMessage (message)
                                .withButton (okText)
                                .withButton (cancelText),
                            [callback] (int result)
                            {
                                // showAsync reports the button index, so the
                                // affirmative button is 0.
                                if (callback)
                                    callback (result == 0);
                            });
}

void showChooserAsync (const String& message,
                       const StringArray& choices, const Array<int>& choiceIds,
                       const String& textLabel,
                       std::function<void (int, String)> onOk)
{
    jassert (choices.size() == choiceIds.size());

    auto* aw = new AlertWindow (dialogTitle, message, MessageBoxIconType::InfoIcon);

    // AlertWindow doesn't take ownership of custom components, so the combo box is
    // kept alive by the callback and freed when the callback itself is destroyed —
    // which happens after the AlertWindow has gone.
    std::shared_ptr<ComboBox> combo;

    if (choices.size() > 0)
    {
        combo = std::make_shared<ComboBox>();
        combo->setSize (300, 26);

        for (int i = 0; i < choices.size(); i++)
            combo->addItem (choices[i], choiceIds[i]);

        combo->setSelectedItemIndex (0);
        aw->addCustomComponent (combo.get());
    }

    const bool withText = textLabel.isNotEmpty();

    if (withText)
        aw->addTextEditor ("name", "", textLabel);

    aw->addButton ("ok", 1);
    aw->addButton ("cancel", 2);

    aw->enterModalState (true, ModalCallbackFunction::create (
        [aw, combo, withText, onOk] (int result)
        {
            if (result != 1)
                return;

            auto text = withText ? aw->getTextEditorContents ("name").trim() : String();

            if (withText && text.isEmpty())
                return;

            if (onOk)
                onOk (combo != nullptr ? combo->getSelectedId() : 0, text);
        }), true);
}
