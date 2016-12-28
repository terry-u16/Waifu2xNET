using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Waifu2xNET.CLR;

namespace Waifu2xNET.Console
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                System.Console.WriteLine("Usage : Waifu2xNET <in>");
                System.Console.WriteLine("Press any key...");
                System.Console.ReadKey();
                return;
            }

            using (var converter = new Waifu2xConverter(GpuMode.Auto))
            {
                System.Console.WriteLine("Initialized.");

                foreach (var path in args)
                {
                    var directory = Path.GetDirectoryName(path);
                    var filename = Path.GetFileName(path);
                    var newFilePath = Path.Combine(directory, $"Waifu2xNET_{filename}").ToString();
                    System.Console.WriteLine($"Converting... : {path}");
                    converter.ConvertFileAsync(path, newFilePath,
                        DenoiseLevel.Level1, 2.0, 128).Wait();
                    System.Console.WriteLine($"Converted : {newFilePath}");
                }
                System.Console.WriteLine("Completed.");
                System.Console.WriteLine("Press any key...");
                System.Console.ReadKey();
            }
        }
    }
}
