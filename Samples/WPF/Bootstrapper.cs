using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Waifu2xNET.Samples.WPF.Views;
using Waifu2xNET.Samples.WPF.Services;
using Prism.Unity;
using System.Windows;
using Prism.Mvvm;
using Microsoft.Practices.Unity;

namespace Waifu2xNET.Samples.WPF
{
    class Bootstrapper : UnityBootstrapper
    {
        protected override DependencyObject CreateShell()
        {
            return Container.TryResolve<Shell>();
        }

        protected override void InitializeShell()
        {
            ViewModelLocator.SetAutoWireViewModel(Shell, true);
            ((Window)Shell).Show();
        }

        protected override void ConfigureContainer()
        {
            Container.RegisterType<IFileOpenDialogService, FileOpenDialogService>();
            base.ConfigureContainer();
        }
    }
}
