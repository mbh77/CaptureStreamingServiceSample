using CommunityToolkit.Mvvm.ComponentModel;
using System.Windows;

namespace MvpMvvm.Dialogs
{
    public class DialogViewModelBase : ObservableValidator
    {
        protected Window? _parent = null;
        public string Title { get; set; } = string.Empty;
        public IDialogResult DialogResult = new DialogResult();

        public void SetParent(Window parent)
        {
            _parent = parent;
        }

        public virtual void OnDialogOpend(IDialogParameters? parameters)
        {
        }
    }
}
