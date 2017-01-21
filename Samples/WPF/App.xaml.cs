using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using Prism.Mvvm;
using Microsoft.Practices.Unity;
using Waifu2xNET.Samples.WPF.Views;

namespace Waifu2xNET.Samples.WPF
{
    /// <summary>
    /// App.xaml の相互作用ロジック
    /// </summary>
    public partial class App : Application
    {
        IUnityContainer Container { get; } = new UnityContainer();

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            new Bootstrapper().Run();
        }
    }
}
