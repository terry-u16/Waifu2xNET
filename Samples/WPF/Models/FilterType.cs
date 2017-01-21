using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Waifu2xNET.CLR;
using System.Text.RegularExpressions;

namespace Waifu2xNET.Samples.WPF.Models
{
    enum FilterType
    {
        Auto,
        None,
        Level1,
        Level2
    }

    static class FilterTypeHelper
    {
        public static DenoiseLevel GetDenoiseLevel(FilterType filterType, string path)
        {
            switch (filterType)
            {
                case FilterType.Auto:
                    if (Regex.IsMatch(path, "\\.jpe?g$", RegexOptions.IgnoreCase))
                        return DenoiseLevel.Level1;
                    else
                        return DenoiseLevel.None;
                case FilterType.None:
                    return DenoiseLevel.None;
                case FilterType.Level1:
                    return DenoiseLevel.Level1;
                case FilterType.Level2:
                    return DenoiseLevel.Level2;
                default:
                    return DenoiseLevel.None;
            }
        }
    }
}
