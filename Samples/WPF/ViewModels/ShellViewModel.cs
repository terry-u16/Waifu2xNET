using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Media.Imaging;
using System.Reactive.Linq;
using Reactive.Bindings;
using Windows.UI.Notifications;
using Prism.Mvvm;
using Waifu2xNET.CLR;
using Waifu2xNET.Samples.WPF.Models;
using Waifu2xNET.Samples.WPF.Services;
using Microsoft.Practices.Unity;

namespace Waifu2xNET.Samples.WPF.ViewModels
{
    class ShellViewModel : BindableBase
    {
        public ReactiveProperty<string> SourceImagePath { get; } = new ReactiveProperty<string>(string.Empty);
        public ReactiveProperty<string> DestinationImagePath { get; }

        public ReadOnlyReactiveProperty<BitmapSource> SourceImage { get; }
        public ReactiveProperty<BitmapSource> ConvertedImage { get; }

        public ReadOnlyReactiveProperty<string> ConvertedImageWidth { get; }
        public ReadOnlyReactiveProperty<string> ConvertedImageHeight { get; }

        public ReactiveProperty<bool> IsConverting { get; } = new ReactiveProperty<bool>(false);

        private FilterType filterType = FilterType.Auto;
        public FilterType FilterType
        {
            get => filterType;
            set => SetProperty(ref filterType, value);
        }

        public ReactiveProperty<string> ScalingText { get; } = new ReactiveProperty<string>("2.00");

        public ReadOnlyReactiveProperty<double?> Scaling { get; }

        public ReactiveProperty<string> ConvertButtonLabel { get; }

        public ReactiveCommand ConvertCommand { get; }

        public ReactiveCommand OpenFileOpenDialogCommand { get; }

        IFileOpenDialogService fileOpenDialogService;

        public ShellViewModel() :this(null)
        {
        }

        [InjectionConstructor]
        public ShellViewModel(IFileOpenDialogService fileOpenDialogService)
        {
            this.fileOpenDialogService = fileOpenDialogService;

            // LoadImage
            SourceImage = SourceImagePath
                .DistinctUntilChanged()
                .Select(path =>
                {
                    if (File.Exists(path))
                    {
                        try
                        {
                            using (var stream = new FileStream(path, FileMode.Open, FileAccess.Read))
                            {
                                var decoder = BitmapDecoder.Create(stream, BitmapCreateOptions.None, BitmapCacheOption.Default);
                                var source = new WriteableBitmap(decoder.Frames[0]);
                                source.Freeze();
                                return source as BitmapSource;
                            }
                        }
                        catch (Exception ex)
                        {
                            // The selected file is not an image, etc...
                            System.Diagnostics.Debug.WriteLine(ex);
                            return null;
                        }
                    }
                    else
                    {
                        return null;
                    }
                }).ToReadOnlyReactiveProperty();

            Scaling = ScalingText
                .Select(s => double.TryParse(s, out double result) ? (double?)result : null)
                .ToReadOnlyReactiveProperty();

            ConvertedImage = Observable.CombineLatest(SourceImage, Scaling, (image, scale) => new { image, scale })
                .Throttle(TimeSpan.FromMilliseconds(300))
                .DistinctUntilChanged()
                .Select(_ => (BitmapSource)null)
                .ToReactiveProperty();

            ConvertedImageWidth = SourceImage
                .CombineLatest(Scaling, (image, scale) => new { image, scale })
                .Select(p => p.image != null ? (p.image.PixelWidth * (p.scale ?? 1)).ToString("0") : string.Empty)
                .ToReadOnlyReactiveProperty();
            ConvertedImageHeight = SourceImage
                .CombineLatest(Scaling, (image, scale) => new { image, scale })
                .Select(p => p.image != null ? (p.image.PixelHeight * (p.scale ?? 1)).ToString("0") : string.Empty)
                .ToReadOnlyReactiveProperty();

            ConvertButtonLabel = IsConverting
                .Select(b => b ? "Converting..." : "Convert")
                .ToReactiveProperty();

            DestinationImagePath = Observable.CombineLatest(SourceImagePath, Scaling,
                (path, scaling) => new { path, scaling })
                .Where(c => c.scaling.HasValue && c.scaling.Value > 0)
                .Select(c =>
                {
                    try
                    {
                        var directory = Path.GetDirectoryName(c.path);
                        var filename = Path.GetFileNameWithoutExtension(c.path);
                        return Path.Combine(directory, $"{filename}_Waifu2xWPF_x{c.scaling:0.00}.png");
                    }
                    catch (Exception)
                    {
                        return string.Empty;
                    }
                })
                .ToReactiveProperty(string.Empty);

            OpenFileOpenDialogCommand = IsConverting
                .Select(b => !b)
                .ToReactiveCommand();

            OpenFileOpenDialogCommand.Subscribe(_ =>
            {
                fileOpenDialogService.Filter = "Image files(*.png,*.jpg,*.bmp,*.gif,*.ico,*.tiff)|*.png;*.jpg;*.bmp;*.gif,*.ico,*.tiff|All files(*.*)|*.*";
                if (fileOpenDialogService.ShowDialog() == true)
                {
                    SourceImagePath.Value = fileOpenDialogService.FileName;
                }
            });

            // ConvertCommand
            ConvertCommand = Observable.CombineLatest(IsConverting, SourceImage, Scaling,
                (converting, image, scaling) => new { converting, image, scaling })
                .Select(cond => !cond.converting && cond.image != null && cond.scaling.HasValue && cond.scaling > 0)
                .ToReactiveCommand(false);

            ConvertCommand.Subscribe(async _ =>
            {
                ConvertedImage.Value = null;
                IsConverting.Value = true;
                using (var converter = new Waifu2xConverter(GpuMode.Auto))
                {
                    ConvertedImage.Value = await converter.ConvertAsync(SourceImage.Value, FilterTypeHelper.GetDenoiseLevel(FilterType, SourceImagePath.Value), Scaling.Value.Value);
                }

                using (var writer = new FileStream(DestinationImagePath.Value, FileMode.Create))
                {
                    var encoder = new PngBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(ConvertedImage.Value));
                    encoder.Save(writer);
                }

                var xml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastImageAndText02);
                var images = xml.GetElementsByTagName("image");
                var src = images[0].Attributes.GetNamedItem("src");
                src.InnerText = "file:///" + DestinationImagePath.Value;

                xml.GetElementsByTagName("text")[0].AppendChild(xml.CreateTextNode("Conversion completed!"));
                xml.GetElementsByTagName("text")[1].AppendChild(xml.CreateTextNode(DestinationImagePath.Value));

                var toast = new ToastNotification(xml);
                ToastNotificationManager.CreateToastNotifier("Waifu2xWPF").Show(toast);

                IsConverting.Value = false;
            });
        }

        public void AddDragAndDropsFiles(IEnumerable<string> paths)
        {
            if (!IsConverting.Value)
            {
                this.SourceImagePath.Value = paths.First();
            }
        }
    }
}
