/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XCShaderCompiler
{
    class Program
    {
        static string[] supportedExtensions = { ".hlsl" };

        static void Main(string[] args)
        {
            Console.Out.WriteLine("XCShader HLSL Compiler");
            CompileShadersRecursively(args[0]);

            Console.ReadLine();
        }

        static void CompileShadersRecursively(string directory)
        {
            //Call recursively for every directory
            foreach (string subDirectory in Directory.GetDirectories(directory))
            {
                CompileShadersRecursively(subDirectory);
            }

            //See if any file exists
            foreach(string fileName in Directory.GetFiles(directory))
            {
                CompileShader(fileName);
            }
        }

        //fxc.exe /Zi /E"EntryPoint" /Od /Fo"ObjectFilePath.cso" /T vs_5_0 /nologo <shaderFilePath>

        static void CompileShader(string filePath)
        {
            string ext = Path.GetExtension(filePath);
            string fileName = Path.GetFileNameWithoutExtension(filePath);

            if (supportedExtensions.Contains(ext))
            {
                //Detect vs/ps
                string profile = "";
                string shaderTypeMain = "";

                if(fileName.ToLower().EndsWith("vs"))
                {
                    profile = "vs_5_0";
                    shaderTypeMain = "VSMain";
                }
                else if(fileName.ToLower().EndsWith("ps"))
                {
                    profile = "ps_5_0";
                    shaderTypeMain = "PSMain";
                }

                if (!profile.Equals("") && !shaderTypeMain.Equals(""))
                {
                    Console.Out.WriteLine("\nCompiling " + fileName);

                    StringCollection values = new StringCollection();
                    string outputObjectPath = filePath.Replace(Path.GetExtension(filePath), ".cso");
                    string processParams = "/Od /Zi /Gfp /E\"" + shaderTypeMain + "\" /Fo\"" + outputObjectPath + "\" /T " + profile + " /nologo \"" + filePath + "\"";

                    ProcessStartInfo startInfo = new ProcessStartInfo();
                    startInfo.FileName = "fxc.exe";
                    startInfo.Arguments = processParams;
                    startInfo.UseShellExecute = false;
                    startInfo.RedirectStandardOutput = true;
                    startInfo.RedirectStandardError = true;

                    Process fxcProcess = Process.Start(startInfo);
                    fxcProcess.OutputDataReceived += (s, e) =>
                    {
                        lock (values)
                        {
                            values.Add(e.Data);
                        }
                    };

                    fxcProcess.ErrorDataReceived += (s, e) =>
                    {
                        lock (values)
                        {
                            values.Add(e.Data);
                        }
                    };

                    fxcProcess.BeginErrorReadLine();
                    fxcProcess.BeginOutputReadLine();
                    fxcProcess.WaitForExit();

                    foreach (string line in values)
                    {
                        Console.Out.WriteLine(line);
                    }
                }
                else
                {
                    Console.Out.WriteLine("Unkown Shader profile and shader type : " + filePath);
                }
            }
        }
    }
}
