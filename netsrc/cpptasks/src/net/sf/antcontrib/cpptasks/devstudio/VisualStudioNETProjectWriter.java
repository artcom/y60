/*
 *
 * Copyright 2004 The Ant-Contrib project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package net.sf.antcontrib.cpptasks.devstudio;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Vector;

import net.sf.antcontrib.cpptasks.CCTask;
import net.sf.antcontrib.cpptasks.CUtil;
import net.sf.antcontrib.cpptasks.TargetInfo;
import net.sf.antcontrib.cpptasks.compiler.CommandLineCompilerConfiguration;
import net.sf.antcontrib.cpptasks.compiler.ProcessorConfiguration;
import net.sf.antcontrib.cpptasks.ide.ProjectDef;
import net.sf.antcontrib.cpptasks.ide.ProjectWriter;

import org.apache.tools.ant.BuildException;
import org.apache.xml.serialize.OutputFormat;
import org.apache.xml.serialize.Serializer;
import org.apache.xml.serialize.XMLSerializer;
import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

/**
 * Writes a Visual Studio.NET project file.
 * (Visual Studio 5 and 6 project writer is substantially more
* complete at this point).
 * @author curta
 *
 */
public final class VisualStudioNETProjectWriter
    implements ProjectWriter {
  /**
   * Version of VisualStudio.NET.
   */
  private String version;

  /**
   * Constructor.
   * @param versionArg String VisualStudio.NET version
   */
  public VisualStudioNETProjectWriter(final String versionArg) {
    this.version = versionArg;
  }

  /**
   * Writes a project definition file.
   *
   * @param fileName
   *            project name for file, should has .cbx extension
   * @param task
   *            cc task for which to write project
   * @param projectDef
   *            project element
   * @param targets compilation targets
   * @param linkTarget link target
   * @throws IOException if I/O error
   * @throws SAXException if XML serialization error
   */
  public void writeProject(final File fileName,
                           final CCTask task,
                           final ProjectDef projectDef,
                           final Hashtable targets,
                           final TargetInfo linkTarget) throws
      IOException,
      SAXException {

    String projectName = projectDef.getName();
    if (projectName == null) {
      projectName = fileName.getName();
    }


    File dspFile = new File(fileName + ".dsp");
    if (!projectDef.getOverwrite() && dspFile.exists()) {
      throw new BuildException("Not allowed to overwrite project file "
                               + dspFile.toString());
    }
    File dswFile = new File(fileName + ".dsw");
    if (!projectDef.getOverwrite() && dswFile.exists()) {
        throw new BuildException("Not allowed to overwrite project file "
                                 + dswFile.toString());
      }

    CommandLineCompilerConfiguration compilerConfig =
        getBaseCompilerConfiguration(targets);
    if (compilerConfig == null) {
      throw new BuildException(
          "Unable to generate Visual Studio.NET project "
          + "when Microsoft C++ is not used.");
    }

    OutputStream outStream = new FileOutputStream(fileName + ".vcproj");
    OutputFormat format = new OutputFormat("xml", "UTF-8", true);
    Serializer serializer = new XMLSerializer(outStream, format);
    ContentHandler content = serializer.asContentHandler();
    String basePath = fileName.getParentFile().getAbsolutePath();
    content.startDocument();
    AttributesImpl emptyAttrs = new AttributesImpl();
    startElement(content, "VisualStudioProject", new String[] {"ProjectType",
                 "Version", "Name", "SccProjectName", "SccLocalPath"}
                 ,
                 new String[] {"Visual C++", this.version, projectName,
                 "", ""});
    content.startElement(null, "Platforms", "Platforms", emptyAttrs);
    startElement(content, "Platform", new String[] {"Name"}
                 , new String[] {"Win32"});
    content.endElement(null, "Platform", "Platform");
    content.endElement(null, "Platforms", "Platforms");
    content.startElement(null, "Configurations", "Configurations", emptyAttrs);
    startElement(content, "Configuration",
                 new String[] {"Name", "OutputDirectory",
                 "IntermediateDirectory", "ConfigurationType", "UseOfMFC",
                 "ATLMinimizeCRunTimeLibraryUsage"}
                 ,
                 new String[] {"Debug|Win32", ".\\Debug", ".\\Debug", "2", "0",
                 "FALSE"});
    String[] clValues = new String[] {
        "VCCLCompilerTool", "0", null, null,
        "1", "2", ".\\Debug\\testdllproh.pch", ".\\Debug/",
        ".\\Debug/", ".\\Debug/", "3", "TRUE", "4"};
    StringBuffer includeDirs = new StringBuffer();
    StringBuffer defines = new StringBuffer();
    String[] args = compilerConfig.getPreArguments();
    for (int i = 0; i < args.length; i++) {
      if (args[i].startsWith("/I")) {
        includeDirs.append(args[i].substring(2));
        includeDirs.append(';');
      }
      if (args[i].startsWith("/D")) {
      	defines.append(args[i].substring(2));
      	defines.append(";");
      }
    }

    if (includeDirs.length() > 0) {
      includeDirs.setLength(includeDirs.length() - 1);
    }
    if (defines.length() > 0) {
    	defines.setLength(defines.length() - 1);
    }
    clValues[2] = includeDirs.toString();
    clValues[3] = defines.toString();

    startElement(content, "Tool",
                 new String[] {"Name", "Optimization",
    		     "AdditionalIncludeDirectories",
                 "PreprocessorDefinitions", "RuntimeLibrary",
                 "UsePrecompiledHeaders", "PrecompiledHeaderFile",
                 "AssemblerListingLocation", "ObjectFile", "WarningLevel",
                 "SuppressStartupBanner", "DebugInformationFormat"}
                 , clValues);
    content.endElement(null, "Tool", "Tool");
    startElement(content, "Tool",
                 new String[] {"Name", "OutputFile", "LinkIncremental",
                 "SuppressStartupBanner", "GenerateDebugInformation",
                 "ProgramDatabaseFile", "SubSystem", "ImportLibrary",
                 "TargetMachine"}
                 ,
                 new String[] {"VCLinkerTool", ".\\Debug/testdllproj.dll", "1",
                 "TRUE", "TRUE", ".\\Debug\\testdllproh.pdb", "2",
                 ".\\Debug/testdllproj.lib", "1"});
    content.endElement(null, "Tool", "Tool");
    content.endElement(null, "Configuration", "Configuration");
    content.endElement(null, "Configurations", "Configurations");
    content.startElement(null, "References", "References", emptyAttrs);
    content.endElement(null, "References", "References");
    content.startElement(null, "Files", "Files", emptyAttrs);

    AttributesImpl fileAttrs = new AttributesImpl();
    fileAttrs.addAttribute(null, "RelativePath", "RelativePath",
                           "#PCDATA", "");

    AttributesImpl fileConfigAttrs = new AttributesImpl();
    fileConfigAttrs.addAttribute(null, "Name", "Name",
                                 "#PCDATA", "Debug|Win32");

    AttributesImpl toolAttrs = new AttributesImpl();
    toolAttrs.addAttribute(null, "Name", "Name",
                           "#PCDATA", "VCCLCompilerTool");
    toolAttrs.addAttribute(null, "Optimization", "Optimization",
                           "#PCDATA", "0");
    toolAttrs.addAttribute(null, "PreprocessorDefinitions",
                           "PreprocessorDefinitions", "#PCDATA",
                           "WIN32;_DEBUG;_WINDOWS;$(NoInherit}");

    Vector sourceList = new Vector(targets.size());
    Iterator targetIter = targets.values().iterator();
    while (targetIter.hasNext()) {
      TargetInfo info = (TargetInfo) targetIter.next();
      File[] sources = info.getSources();
      for (int i = 0; i < sources.length; i++) {
        sourceList.addElement(sources[i]);
      }
    }
    Object[] sortedSources = new File[sourceList.size()];
    sourceList.copyInto(sortedSources);
    Arrays.sort(sortedSources, new Comparator() {
      public int compare(final Object o1, final Object o2) {
        return ((File) o1).getName().compareTo(((File) o2).getName());
      }
    });

    for (int i = 0; i < sortedSources.length; i++) {
      String relativePath = CUtil.getRelativePath(basePath,
                                                  (File) sortedSources[i]);
      fileAttrs.setValue(0, relativePath);
      content.startElement(null, "File", "File", fileAttrs);
      content.startElement(null, "FileConfiguration", "FileConfiguration",
                           fileConfigAttrs);
      content.startElement(null, "Tool", "Tool", toolAttrs);
      content.endElement(null, "Tool", "Tool");
      content.endElement(null, "FileConfiguration", "FileConfiguration");
      content.endElement(null, "File", "File");
    }
    content.endElement(null, "Files", "Files");
    content.startElement(null, "Globals", "Globals", emptyAttrs);
    content.endElement(null, "Globals", "Globals");
    content.endElement(null, "VisualStudioProject", "VisualStudioProject");
    content.endDocument();
  }

  /**
   * Start an element.
   * @param content ContentHandler content handler
   * @param tagName String tag name
   * @param attributeNames String[] attribute names
   * @param attributeValues String[] attribute values
   * @throws SAXException if error writing element
   */
  private void startElement(final ContentHandler content,
                                  final String tagName,
                                  final String[] attributeNames,
                                  final String[] attributeValues)
      throws SAXException {
    AttributesImpl attributes = new AttributesImpl();
    for (int i = 0; i < attributeNames.length; i++) {
      attributes.addAttribute(null, attributeNames[i],
                              attributeNames[i], "#PCDATA", attributeValues[i]);
    }
    content.startElement(null, tagName, tagName, attributes);
  }

  /**
   * Gets the first recognized compiler from the
   * compilation targets.
   * @param targets compilation targets
   * @return representative (hopefully) compiler configuration
   */
  private CommandLineCompilerConfiguration
      getBaseCompilerConfiguration(final Hashtable targets) {
    //
    //   find first target with an DevStudio C compilation
    //
    CommandLineCompilerConfiguration compilerConfig = null;
    //
    //   get the first target and assume that it is representative
    //
    Iterator targetIter = targets.values().iterator();
    while (targetIter.hasNext()) {
      TargetInfo targetInfo = (TargetInfo) targetIter.next();
      ProcessorConfiguration config = targetInfo.getConfiguration();
      String identifier = config.getIdentifier();
      //
      //   for the first cl compiler
      //
      if (config instanceof CommandLineCompilerConfiguration) {
        compilerConfig = (CommandLineCompilerConfiguration) config;
        if (compilerConfig.getCompiler() instanceof DevStudioCCompiler) {
          return compilerConfig;
        }
      }
    }
    return null;
  }

}
