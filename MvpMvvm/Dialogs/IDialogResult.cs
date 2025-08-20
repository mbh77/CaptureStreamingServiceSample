namespace MvpMvvm.Dialogs
{
    public interface IDialogResult
    {
        DialogButtonResult Result { get; }
        IDialogParameters Parameters { get; }
    }
}
