Fallout 4 Script Extender v0.6.21
by Ian Patterson, Stephen Abel, and Brendan Borthwick (ianpatt, behippo, and plb)

The Fallout 4 Script Extender, or F4SE for short, is a modder's resource that expands the scripting capabilities of Fallout 4. It does so without modifying the executable files on disk, so there are no permanent side effects.

Compatibility:

F4SE will support the latest version of Fallout available on Steam, and _only_ this version. When a new version is released, we'll update as soon as possible: please be patient. Don't email asking when the update will be ready; we already know the new version is out. The editor does not currently need modification, however when available a custom set of .pex/psc files must be installed.

This version is compatible with runtime 1.10.163. You can ignore any part of the version number after those components; they're not relevant for compatibility purposes.

[ Installation ]

1. Copy f4se_1_10_163.dll, f4se_loader.exe, and f4se_steam_loader.dll to your Fallout installation folder. This is usually C:\Program Files (x86)\Steam\SteamApps\common\Fallout 4\, but if you have installed to a custom Steam library then you will need to find this folder yourself. Copy the Data folder over as well.

2. Launch the game by running the copy of f4se_loader.exe that you placed in the Fallout installation folder.

3. If you are looking for information about a specific feature, check f4se_whatsnew.txt for more details.

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

Entire Team
Send email to team [at] f4se [dot] silverlock [dot] org

Ian (ianpatt)
Send email to ianpatt+f4se [at] gmail [dot] com

Stephen (behippo)
Send email to gamer [at] silverlock [dot] org

[ Standard Disclaimer ]

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
