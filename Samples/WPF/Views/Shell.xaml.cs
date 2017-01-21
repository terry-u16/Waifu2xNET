using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Waifu2xNET.Samples.WPF.ViewModels;

namespace Waifu2xNET.Samples.WPF.Views
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class Shell : Window
    {
        public Shell()
        {
            InitializeComponent();
        }

        private ShellViewModel ViewModel
        {
            get { return this.DataContext as ShellViewModel; }
        }

        private void Window_Drop(object sender, DragEventArgs e)
        {
            var dropFileList = e.Data.GetData(DataFormats.FileDrop) as string[];
            ViewModel.AddDragAndDropsFiles(dropFileList);
        }

        private void Window_DragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop) 
                && File.Exists((e.Data.GetData(DataFormats.FileDrop) as string[])[0]))
            {
                e.Effects = DragDropEffects.Link;
            }
            else
            {
                e.Effects = DragDropEffects.None;
            }

            e.Handled = true;
        }
    }
}
