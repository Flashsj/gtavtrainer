using System;
using System.IO;
using System.Threading;
using System.Collections.Generic;
using System.Linq;

namespace process
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                var input = File.ReadAllLines($"C:\\Users\\{Environment.UserName}\\AppData\\Roaming\\GTAVTRAINER");
                var output = new List<string>();
                var count = new Dictionary<string, int>();

                foreach (var line in input)
                {
                    var t = line.Split("\",".ToCharArray(), StringSplitOptions.RemoveEmptyEntries).ToList();
                    if (t.Count > 3 && t[1] == "NATIVE")
                    {
                        if (!count.ContainsKey(t[3]))
                            count[t[3]] = 1;
                        count[t[3]]++;
                    }
                }

                foreach (var key in count.Keys) { output.Add(key + "," + count[key].ToString()); }

                if (File.Exists(input.ToString()))
                    File.WriteAllLines(input.ToString(), output);
                else throw new Exception("Unable to write memory");
            }
            catch (Exception ex) { throw new Exception($"Fatal runtime error: {ex.Message}"); }
        }
    }
}
