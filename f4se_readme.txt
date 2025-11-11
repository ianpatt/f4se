Fallout 4 Script Extender v0.7.4 BETA
by Ian Patterson, Stephen Abel, and Brendan Borthwick (ianpatt, behippo, and plb)

The Fallout 4 Script Extender, or F4SE for short, is a modder's resource that expands the scripting capabilities of Fallout 4. It does so without modifying the executable files on disk, so there are no permanent side effects.

#####################################################################
All native code plugins (.dlls) need to be updated by their
creators for 1.10.980+.

Old native code plugins not loading is not a problem with F4SE.
#####################################################################

Compatibility:

F4SE will support the latest version of Fallout available on Steam and GOG, and _only_ these versions. The MS Store/Gamepass/Epic are not supported. When a new version is released, we'll update as soon as possible: please be patient. Don't email asking when the update will be ready; we already know the new version is out. The editor does not currently need modification, however when available a custom set of .pex/psc files must be installed.

This version is compatible with runtime 1.11.137. You can ignore any part of the version number after those components; they're not relevant for compatibility purposes.

[ Installation ]

1. Copy f4se_1_11_137.dll and f4se_loader.exe to your Fallout installation folder. This is usually C:\Program Files (x86)\Steam\SteamApps\common\Fallout 4\, but if you have installed to a custom Steam library then you will need to find this folder yourself. Copy the Data folder over as well.

2. Ignore the 'src' folder. It is for programmers.

3. Launch the game by running the copy of f4se_loader.exe that you placed in the Fallout installation folder.

4. If you are looking for information about a specific feature, check f4se_whatsnew.txt for more details.

[ FAQ ]

* Console version?
 - No. Not possible due to policy restrictions put in place by the console manufacturers.

* My virus scanner complains about f4se_loader. Is it a virus?
 - No. DLL injection sets off badly-written virus scanners. Nothing we can do about it.

* Can I redistribute or make modified versions of F4SE?
 - No. The suggested method for extending F4SE is to write a plugin. If this does not meet your needs, please email the contact addresses listed below.

* How do I write a plugin for F4SE?
 - There is no example plugin yet, so you'll need to look at PluginAPI.h for details on the high-level interface. The API is not currently locked down due to the early state of the project, so anything may change in later versions. Note that plugins must have their source code publicly available.

* How do I uninstall F4SE?
 - Delete the three files you copied to your Fallout installation folder.

* How much does this cost?
 - F4SE is free.

[ Contact ]

If you are having trouble with the installation instructions or are running in to other problems, post on /r/f4se. We cannot help you solve load order problems.

THIS IS A VERSION FOR MOD DEVELOPER TESTING ONLY. General reports that a mod doesn't work or crashes that require a complicated mod list are not helpful at this time. Even vanilla 1.10.980 appears to be unstable. Reports like "when I call GetWaterType on this specific cell with editor ID [whatever] it returns the wrong result" would be ideal, but I'll be doing investigation as needed. Please keep in mind that I am one person, and there are a *lot* of mods out there.

Ian (ianpatt)
Send email to ianpatt+f4setest [at] gmail [dot] com

[ Standard Disclaimer ]

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
