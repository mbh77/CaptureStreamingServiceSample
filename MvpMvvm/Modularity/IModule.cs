using Microsoft.Extensions.DependencyInjection;

namespace MvpMvvm.Modularity
{
    public interface IModule
    {
        void RegisterTypes(IServiceCollection services);
        void OnInitialized(IServiceProvider serviceProvider);
    }
}
