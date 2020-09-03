#
# This file is an aid to generate the Languages rules file.
# usage:
#   python languages.py > ..\winbuild\Languages.rules
#
import os
import re

files = [f for f in os.listdir('.') if re.match(r'translator_[a-z][a-z]\.h', f)]
new_list = []
for f in files:
    new_list.append([f,(os.path.splitext(f)[0]).replace("translator_","").upper()])

#
# generating file is lang_cfg.py
# the rules file has to output lang_cfg.h
#
print("""\
<?xml version="1.0" encoding="utf-8"?>
<VisualStudioToolFile
        Name="languages"
        Version="8.00"
        >
        <Rules>
                <CustomBuildRule
                        Name="Languages"
                        DisplayName="Settings"
                        CommandLine="python $(InputPath) [AllOptions] [AdditionalOptions] &gt; $(InpDir)/$(InputName).h"
                        Outputs="$(IntDir)/$(InputName).h"
                        FileExtensions="*.py"
                        AdditionalDependencies=""
                        ExecutionDescription="Executing languages ..."
                        ShowOnlyRuleProperties="false"
                        >
                        <Properties>
                               <EnumProperty
                                       Name="EnglishOnly"
                                       DisplayName="Use English Only"
                                       Description="Use English Only"
                                       DefaultValue="0"
                               >
                                       <Values>
                                               <EnumValue
                                               Value="0"
                                               Switch=""
                                               DisplayName="Don't use English Only"
                                               />
                                               <EnumValue
                                               Value="1"
                                               Switch="ENONLY"
                                               DisplayName="Use English Only"
                                               />
                                       </Values>
                               </EnumProperty>
""")
#
# generate loop, English is mandatory (so cannot be chosen)
#
for f in new_list:
    if (f[1] != "EN"):
        # search for the language description
        fil = open(f[0], 'r')
        tmp = ""
        for line in fil:
          if "idLanguage" in line:
             tmp = line
             if "}" in line:
               break
          elif (tmp != ""):
             tmp += line
             if "}" in line:
               break
    
        tmp = tmp.replace("\n","")
        l = re.sub('[^"]*"([^"]*)".*','\\1',tmp)
        l1 = l.replace("-","")
        # capitalize first letter
        l = l.title()
        print("""\
                                   <EnumProperty
                                           Name="%s"
                                           DisplayName="Use %s"
                                           Description="Use %s"
                                           DefaultValue="1"
                                   >
                                           <Values>
                                                   <EnumValue
                                                   Value="0"
                                                   Switch=""
                                                   DisplayName="Don't use %s"
                                                   />
                                                   <EnumValue
                                                   Value="1"
                                                   Switch="%s"
                                                   DisplayName="Use %s"
                                                   />
                                           </Values>
                                   </EnumProperty>
        """ % (l1, l, l, l, f[1], l))

print("""\
                        </Properties>
                </CustomBuildRule>
        </Rules>
</VisualStudioToolFile>
""")
