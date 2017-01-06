using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Waifu2xNET.Samples.WPF.Views;
using Prism.Unity;
using System.Windows;

namespace Waifu2xNET.Samples.WPF
{
    class Bootstrapper : UnityBootstrapper
    {
        protected override DependencyObject CreateShell()
        {
            return this.Container.TryResolve<Shell>();
        }

        protected override void InitializeShell()
        {
            ((Window)this.Shell).Show();
        }
    }
}
