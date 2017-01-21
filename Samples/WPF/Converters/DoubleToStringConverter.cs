using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace Waifu2xNET.Samples.WPF.Converters
{
    [ValueConversion(typeof(double), typeof(string))]
    class DoubleToStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return ((double)value).ToString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (double.TryParse((string)value, out double x))
            {
                return x;
            }
            else
            {
                return Binding.DoNothing;
            }
        }
    }
}
