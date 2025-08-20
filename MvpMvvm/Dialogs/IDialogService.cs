using System.Windows.Controls;

namespace MvpMvvm.Dialogs
{
    public interface IDialogService
    {
        void RegisterDialog<TViewModel, TView>()
            where TViewModel : DialogViewModelBase
            where TView : Control;
        void ShowDialog<TViewModel>(IDialogParameters? parameters, Action<IDialogResult> callback)
            where TViewModel : DialogViewModelBase;
        void ShowDialog(object? viewModel, IDialogParameters? parameters, Action<IDialogResult> callback);
        DialogWindow? Show<TViewModel>(IDialogParameters? parameters, Action<IDialogResult> callback)
            where TViewModel : DialogViewModelBase;
        DialogWindow? Show(object? viewModel, IDialogParameters? parameters, Action<IDialogResult> callback);
    }
}
