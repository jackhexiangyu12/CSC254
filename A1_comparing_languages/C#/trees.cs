using System;
using System.Collections.Generic;

public class Trees {
    public static List<string> Gen(int n) {
        List<List<string>> result = new List<List<string>>();

        List<string> tmp = new List<string>();
        tmp.Add("");
        result.Add(tmp);

        if (n > 0) {

            tmp = new List<string>();
            tmp.Add("(.)");
            result.Add(tmp);

            for (int i = 2; i < n+1; i++) {

                tmp = new List<string>();
                for (int j = 0; j < i; j++) {
                    foreach (string left in result[j]) {
                        foreach (string right in result[i-j-1]) {
                            tmp.Add("(" + left + "." + right + ")");
                        }
                    }
                }

                result.Add(tmp);
            }
        }

        return result[result.Count - 1];
    }


    public static void Main(string[] args) {
        int n = Int32.Parse(args[0]);
        List<string> trees = Gen(n);
        foreach (string tree in trees) {
            Console.WriteLine(tree);
        }
    }
}
