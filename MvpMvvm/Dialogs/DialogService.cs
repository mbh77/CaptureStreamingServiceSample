using CommunityToolkit.Mvvm.DependencyInjection;
using System.Windows;
using System.Windows.Controls;

namespace MvpMvvm.Dialogs
{
    internal class DialogService : IDialogService
    {
        private static Dictionary<Type, Type> _mappings = new Dictionary<Type, Type>();

        public void RegisterDialog<TViewModel, TView>()
            where TViewModel : DialogViewModelBase
            where TView : Control
        {
            _mappings.Add(typeof(TViewModel), typeof(TView));
        }

        public void ShowDialog<TViewModel>(IDialogParameters? parameters, Action<IDialogResult> callback)
            where TViewModel : DialogViewModelBase
        {
            if (_mappings.TryGetValue(typeof(TViewModel), out var type))
            {
                var vmTypeResolvedInstance = Ioc.Default.GetService(typeof(TViewModel));
                var vmTypeCteatedInstance = Activator.CreateInstance(typeof(TViewModel));

                var viewModel = vmTypeResolvedInstance as DialogViewModelBase ?? vmTypeCteatedInstance as DialogViewModelBase;
                if (viewModel != null)
                {
                    ShowDialogInternal(type, parameters, callback, viewModel);
                }
            }
        }

        public void ShowDialog(object? viewModel, IDialogParameters? parameters, Action<IDialogResult> callback)
        {
            if (viewModel is not null)
            {
                if (_mappings.TryGetValue(viewModel.GetType(), out var type))
                {
                    ShowDialogInternal(type, parameters, callback, viewModel);
                }
            }
        }

        private DialogWindow? ShowDialogInternal(Type? type, IDialogParameters? parameters, Action<IDialogResult> callback, object? viewModel, bool modal=true)
        {
            if (Application.Current.MainWindow is Window mainWindow)
            {
                DialogWindow? dialog = new DialogWindow();
                EventHandler? closeEventHandler = null;
                RoutedEventHandler? openEventHandler = null;

                if (dialog != null && type != null && viewModel != null)
                {
                    var content = Activator.CreateInstance(type);

                    if (viewModel is DialogViewModelBase dialogViewModel)
                    {
                        dialogViewModel.SetParent(dialog);
                        if (content is Control control)
                        {
                            control.DataContext = dialogViewModel;
                            dialog.Content = control;
                            dialog.Title = dialogViewModel.Title;
                        }

                        closeEventHandler = (s, e) =>
                        {
                            callback(dialogViewModel.DialogResult);
                            dialog.Closed -= closeEventHandler;
                        };
                        dialog.Closed += closeEventHandler;

                        openEventHandler = (s, e) =>
                        {
                            dialogViewModel.OnDialogOpend(parameters);
                            dialog.Loaded -= openEventHandler;
                        };
                        dialog.Loaded -= openEventHandler;
                        dialog.Loaded += openEventHandler;
                    }

                    dialog.Owner = mainWindow;
                    if (modal)
                        dialog.ShowDialog();
                    else
                        dialog.Show();
                }
                return modal ? null : dialog;
            }
            return null;
        }

        public DialogWindow? Show<TViewModel>(IDialogParameters? parameters, Action<IDialogResult> callback)
            where TViewModel : DialogViewModelBase
        {
            if (_mappings.TryGetValue(typeof(TViewModel), out var type))
            {
                var vmTypeResolvedInstance = Ioc.Default.GetService(typeof(TViewModel));
                var vmTypeCteatedInstance = Activator.CreateInstance(typeof(TViewModel));

                var viewModel = vmTypeResolvedInstance as DialogViewModelBase ?? vmTypeCteatedInstance as DialogViewModelBase;
                if (viewModel != null)
                {
                    return ShowDialogInternal(type, parameters, callback, viewModel, false);
                }
            }
            return null;
        }

        public DialogWindow? Show(object? viewModel, IDialogParameters? parameters, Action<IDialogResult> callback)
        {
            if (viewModel is not null)
            {
                if (_mappings.TryGetValue(viewModel.GetType(), out var type))
                {
                    return ShowDialogInternal(type, parameters, callback, viewModel, false);
                }
            }
            return null;
        }

    }
}
