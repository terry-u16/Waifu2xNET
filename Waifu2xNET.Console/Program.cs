using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Media.Imaging;
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

            ConvertInMemory(args).Wait();

            System.Console.WriteLine("Press any key...");
            System.Console.ReadKey();
        }

        static async Task ConvertInMemory(string[] pathList)
        {
            DisplayDeviceInfo();

            using (var converter = new Waifu2xConverter(GpuMode.Auto))
            {
                System.Console.WriteLine("Initialized.");

                foreach (var path in pathList)
                {
                    var stopwatch = new System.Diagnostics.Stopwatch();
                    stopwatch.Start();

                    System.Console.WriteLine($"Converting... : {path}");

                    var newFilePath = GetNewFilePath(path);
                    var source = new BitmapImage(new Uri(path, UriKind.RelativeOrAbsolute));
                    source.Freeze();

                    var result = await converter.ConvertAsync(source, DenoiseLevel.Level1, 2.0, 0);
                    
                    using (var writer = new FileStream(newFilePath, FileMode.Create))
                    {
                        var encoder = new PngBitmapEncoder();
                        encoder.Frames.Add(BitmapFrame.Create(result));
                        encoder.Save(writer);
                    }

                    System.Console.WriteLine($"Converted : {newFilePath}");

                    stopwatch.Stop();
                    System.Console.WriteLine($"Elasped time : {stopwatch.Elapsed}");
                }

                System.Console.WriteLine("Completed.");
            }
        }

        static async Task ConvertFromFile(string[] pathList)
        {
            using (var converter = new Waifu2xConverter(GpuMode.Auto))
            {
                System.Console.WriteLine("Initialized.");

                foreach (var path in pathList)
                {
                    System.Console.WriteLine($"Converting... : {path}");
                    var newFilePath = GetNewFilePath(path);
                    await converter.ConvertFileAsync(path, newFilePath, DenoiseLevel.Level1, 2.0);
                    System.Console.WriteLine($"Converted : {newFilePath}");
                }
                System.Console.WriteLine("Completed.");
            }
        }

        static string GetNewFilePath(string path)
        {
            var directory = Path.GetDirectoryName(path);
            var filename = Path.GetFileName(path);
            return Path.Combine(directory, $"Waifu2xNET_{filename}").ToString();
        }

        static void DisplayDeviceInfo()
        {
            foreach (var processor in Waifu2xConverter.Processors)
            {
                System.Console.WriteLine($"[{processor.DeviceName}]");
                System.Console.WriteLine($"DeviceID : {processor.DeviceID}");
                System.Console.WriteLine($"ProcessorType : {processor.ProcessorType}");
                System.Console.WriteLine($"NumberOfCores : {processor.NumberOfCores}");
                System.Console.WriteLine($"SubType : {processor.ProcessorSubType}");
                System.Console.WriteLine();
            }
        }
    }
}
