using Microsoft.Win32;

namespace Waifu2xNET.Samples.WPF.Services
{
    class FileOpenDialogService : IFileOpenDialogService
    {
        OpenFileDialog dialog = new OpenFileDialog();

        public string FileName { get => dialog.FileName; set => dialog.FileName = value; }

        public string[] FileNames => dialog.FileNames;

        public string Filter { get => dialog.Filter; set => dialog.Filter = value; }
        public string Title { get => dialog.Title; set => dialog.Title = value; }
        public bool CanSelectMultiFiles { get => dialog.Multiselect; set => dialog.Multiselect = value; }

        public bool? ShowDialog()
        {
            return dialog.ShowDialog();
        }
    }
}
