using System;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Media.Imaging;
using Waifu2xNET.CLR;

namespace Waifu2xNET.Samples.Console
{
    class Program
    {
        static void Main(string[] args)
        {
            // Show how to use if no file path is passed.
            if (args.Length == 0)
            {
                System.Console.WriteLine("How to use : Waifu2xNET.exe <input file paths>");
                System.Console.WriteLine("Press any key...");
                System.Console.ReadKey();
                return;
            }

            // Convert images from file and wait.
            ConvertFromFile(args).Wait();

            // You can also load/save files manually if you want to.
            ConvertInMemory(args).Wait();

            System.Console.WriteLine("Press any key...");
            System.Console.ReadKey();
        }

        /// <summary>
        /// A sample of converting each image from file.
        /// </summary>
        /// <param name="pathList">List of file paths</param>
        /// <returns></returns>
        static async Task ConvertFromFile(string[] pathList)
        {
            // Initialize waifu2x converter.
            // Using statement is needed to dispose unmanaged resources.
            using (var converter = new Waifu2xConverter(GpuMode.Auto))
            {
                System.Console.WriteLine("Initialized.");

                foreach (var path in pathList)
                {
                    System.Console.WriteLine($"Converting... : {path}");
                    var newFilePath = GetNewFilePath(path);

                    // Convert an image x2 from file and save it asynchronously.
                    await converter.ConvertFileAsync(path, newFilePath, DenoiseLevel.Level1, 2.0);

                    System.Console.WriteLine($"Converted : {newFilePath}");
                }
                System.Console.WriteLine("Completed.");
            }
        }

        /// <summary>
        /// A sample of converting each image in memory.
        /// </summary>
        /// <param name="pathList">List of file paths</param>
        /// <returns></returns>
        static async Task ConvertInMemory(string[] pathList)
        {
            // Display available processors (if you want to).
            DisplayDeviceInfo();

            // Initialize waifu2x converter.
            // Using statement is needed to dispose unmanaged resources.
            using (var converter = new Waifu2xConverter(GpuMode.Auto))
            {
                System.Console.WriteLine("Initialized.");

                foreach (var path in pathList)
                {
                    System.Console.WriteLine($"Converting... : {path}");

                    // Load an image from file.
                    var newFilePath = GetNewFilePath(path);
                    var source = new BitmapImage(new Uri(path, UriKind.RelativeOrAbsolute));
                    source.Freeze();  // Freeze object to convert in other thread.

                    // Convert the image x2 in memory asynchronously.
                    var result = await converter.ConvertAsync(source, DenoiseLevel.Level1, 2.0);
                    
                    // Save the converted image.
                    using (var writer = new FileStream(newFilePath, FileMode.Create))
                    {
                        var encoder = new PngBitmapEncoder();
                        encoder.Frames.Add(BitmapFrame.Create(result));
                        encoder.Save(writer);
                    }

                    System.Console.WriteLine($"Converted : {newFilePath}");
                }

                System.Console.WriteLine("Completed.");
            }
        }

        /// <summary>
        /// Generate a path of the converted image.
        /// ex.) xxx/yyy.jpg -> xxx/Waifu2xNET_yyy.png
        /// </summary>
        /// <param name="path">Path of a source file</param>
        /// <returns></returns>
        static string GetNewFilePath(string path)
        {
            var directory = Path.GetDirectoryName(path);
            var filename = Path.GetFileName(path).Replace(Path.GetExtension(path), ".png");
            return Path.Combine(directory, $"Waifu2xNET_{filename}").ToString();
        }

        /// <summary>
        /// Display available processors.
        /// </summary>
        static void DisplayDeviceInfo()
        {
            // Enumerate available processors.
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
