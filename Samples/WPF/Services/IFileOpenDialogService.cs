using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Waifu2xNET.Samples.WPF.Services
{
    interface IFileOpenDialogService
    {
        string Title { get; set; }

        string Filter { get; set; }

        string FileName { get; set; }

        string[] FileNames { get; }

        bool CanSelectMultiFiles { get; set; }

        bool? ShowDialog();
    }
}
