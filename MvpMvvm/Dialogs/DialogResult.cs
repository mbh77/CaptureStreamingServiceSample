namespace MvpMvvm.Dialogs
{
    public class DialogResult : IDialogResult
    {
        public DialogButtonResult Result { get; set; }

        public IDialogParameters Parameters { get; set; } = new DialogParameters();

        public DialogResult() : this(DialogButtonResult.None)
        {

        }

        public DialogResult(DialogButtonResult result)
        {
            Result = result;
        }
    }
}
