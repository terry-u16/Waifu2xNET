using System;
using System.Collections.Generic;
using System.Xml;

namespace XmlCommentFilter
{
    /// <summary>
    /// XMLドキュメントコメントを用いたドキュメントの作成 | smdn
    /// http://smdn.jp/programming/netfx/classlibrary/2_xmldoccomments/#section.5.2
    /// </summary>
    class Program
    {
        static void Main(string[] args)
        {
            // 第一引数で与えられたXMLファイルを読み込む
            var document = new XmlDocument();

            document.Load(args[0]);

            // 第二引数で与えられるxml:langの値にしたがって要素をフィルタリングする
            var filterLang = 2 <= args.Length ? args[1] : string.Empty;

            // 全ての/doc/members/member要素について、引数で与えられたxml:langの値を持つ要素だけをフィルタリングする
            foreach (XmlElement memberElement in document.SelectNodes("/doc/members/member"))
            {
                // member下の要素名と要素のXmlElementを格納するDictionary
                var langElement = new Dictionary<string, XmlElement>();
                // member下の全てのXmlElementを格納するList
                var elements = new List<XmlElement>();

                // menber要素のすべての子要素を列挙
                foreach (XmlElement element in memberElement.ChildNodes)
                {
                    elements.Add(element);

                    // 要素名を取得
                    var name = element.Name;
                    // xml:lang属性の値を取得
                    var lang = element.GetAttribute("xml:lang");

                    // xml:lang属性を削除する
                    element.RemoveAttribute("xml:lang");

                    if (langElement.ContainsKey(name))
                    {
                        // すでにDictionaryに同名の要素が格納されている場合、
                        // xml:langが目的の値を持つ場合に限り上書きする
                        if (lang == filterLang)
                            langElement[name] = element;
                    }
                    else
                    {
                        langElement[name] = element;
                    }
                }

                // 一旦member要素のすべての子要素を削除する
                foreach (var element in elements)
                {
                    memberElement.RemoveChild(element);
                }

                // フィルタリングした要素だけを追加する
                foreach (var element in langElement.Values)
                {
                    memberElement.AppendChild(element);
                }
            }

            // フィルタリングしたXMLドキュメントを標準出力に書き出す
            var writerSettings = new XmlWriterSettings();

            writerSettings.Indent = true;
            writerSettings.IndentChars = "    ";

            var writer = XmlWriter.Create(Console.OpenStandardOutput(), writerSettings);

            document.Save(writer);

        }
    }
}
