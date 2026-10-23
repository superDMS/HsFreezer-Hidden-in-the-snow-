import psutil,win32process,os,win32gui,win32api,win32con
from pyautogui import  confirm,prompt
#本人在各种能存文件的地方找了好久,终于翻到了某个网盘里躺着的初代版本雪藏v0.01,本来上百行,本人看着几年前写的代码属实受不了,在尽量保证初代雪藏的原汁原味的操作下,稍作凝练优化了一下!
#特此分享用于学习&交流.超级压缩版,异常捕捉都用不着,巧妙避开崩溃,区区40行,在保证进程不崩的同时,既有GUI,冻结与解冻逻辑完整,麻雀虽小五脏俱全,本人一贯喜欢用高级抽象写法,一行代码顶n行的,这是习惯,python极端写法爱好者,见谅.
def find_process_h(exe_name):
    hwnd_list = []
    win32gui.EnumWindows(lambda _hwnd, _hwnd_list: _hwnd_list.append(_hwnd), hwnd_list) 
    for hwnd in hwnd_list:
        if win32process.GetModuleFileNameEx(win32api.OpenProcess(win32con.PROCESS_ALL_ACCESS, False, win32process.GetWindowThreadProcessId(hwnd)[1]),0).find(exe_name) > 0:        
            return hwnd 
#自行排除吧,不然对话框列表装不过来! 尽量把电脑上的无关进程全排除了,留下几个你自己常用的名单!
pass_exe=['ZFGameBrowser.exe', 'amdow.exe', 'audiodg.exe', 'cmd.exe', 'cncmd.exe', 'copyq.exe', 'frpc.exe', 'gamingservicesnet.exe', 'memreduct.exe', 'mmcrashpad_handler64.exe','GameBarPresenceWriter.exe', 'HipsTray.exe', 'HsFreezer.exe', 'HsFreezerMagiaMove.exe', 'PhoneExperienceHost.exe','PixPin.exe', 'PresentMon-x64.exe','msedgewebview2.exe', 'plugin_host-3.3.exe', 'plugin_host-3.8.exe','explorer.exe','System Idle Process', 'System', 'svchost.exe', 'Registry', 'smss.exe', 'csrss.exe', 'wininit.exe', 'winlogon.exe', 'services.exe', 'lsass.exe', 'atiesrxx.exe', 'amdfendrsr.exe', 'atieclxx.exe', 'MemCompression', 'ZhuDongFangYu.exe', 'wsctrlsvc.exe', 'AggregatorHost.exe', 'wlanext.exe', 'conhost.exe', 'spoolsv.exe', 'reWASDService.exe', 'AppleMobileDeviceService.exe', 'ABService.exe', 'mDNSResponder.exe', 'Everything.exe', 'SunloginClient.exe', 'RtkAudUService64.exe', 'gamingservices.exe', 'SearchIndexer.exe', 'MoUsoCoreWorker.exe', 'SecurityHealthService.exe', 'HsFreezerEx.exe', 'GameInputSvc.exe', 'TrafficProt.exe', 'HipsDaemon.exe','python.exe', 'pythonw.exe', 'qmbrowser.exe', 'reWASDEngine.exe', 'sihost.exe', 'sublime_text.exe', 'taskhostw.exe', 'SearchProtocolHost.exe','crash_handler.exe', 'crashpad_handler.exe', 'ctfmon.exe', 'dasHost.exe', 'dllhost.exe', 'dwm.exe', 'fontdrvhost.exe','RuntimeBroker.exe','taskhostw.exe''WeChatAppEx.exe', 'WeChatOCR.exe', 'WeChatPlayer.exe', 'WeChatUtility.exe', 'WidgetService.exe', 'Widgets.exe', 'WmiPrvSE.exe', 'Xmp.exe','QQScreenshot.exe', 'RadeonSoftware.exe', 'SakuraFrpService.exe', 'SakuraLauncher.exe', 'SearchHost.exe', 'SecurityHealthSystray.exe', 'ShellExperienceHost.exe', 'StartMenuExperienceHost.exe', 'SystemSettings.exe', 'SystemSettingsBroker.exe', 'TextInputHost.exe', 'TrafficMonitor.exe', 'UserOOBEBroker.exe','WeChatAppEx.exe','360zipUpdate.exe', 'AMDRSServ.exe', 'AMDRSSrcExt.exe', 'APlayer.exe', 'ApplicationFrameHost.exe', 'CPUMetricsServer.exe', 'ChsIME.exe', 'DownloadSDKServer.exe','QMWeiyun.exe'];save_input=[]
def main():
    while 1:
        while 1:
            now_exe_list =[]
            for proc in psutil.process_iter(attrs=['name']):
                if proc.info['name'] not in  pass_exe and  proc.info['name'] not in now_exe_list:
                    now_exe_list.append(proc.info['name'])
            now_exe_list.sort();print('当前进程:',now_exe_list)
            exe = confirm('请选择对象',title='雪藏HsFreezer v0.01纪念版', buttons= ['刷新列表','手动输入']+save_input+now_exe_list)
            if exe == '刷新列表':pass
            elif exe=='手动输入':
                exe=prompt('手动输入','请输入exe文件名并会在下次的名单中置顶(仅限本次运行):')
                save_input.append(exe)
                break
            elif exe==None:break
            else:break
        select = confirm('待执行操作的EXE为:  ' + exe+'   对其进行何种操作?', buttons=['冻结', '解冻'])
        if select == '冻结':
            win32gui.ShowWindow(find_process_h(select), 0)  #隐藏窗口:由于没做复杂内覆盖处理,可能导致异常,可以尝试使用     
            os.system(f'pssuspend64.exe {exe}')#动用微软pstool工具包里的pssuspend64.exe(与py文件放同个目录下)来行驶冻结功能.#pstool工具包可到微软官方下载:https://learn.microsoft.com/zh-cn/sysinternals/downloads/pstools
        elif select == '解冻':
            os.system(f'pssuspend64.exe -r {exe}')#动用微软pstool工具包里的pssuspend64.exe(与py文件放同个目录下)来行驶解冻功能.
            win32gui.ShowWindow(find_process_h(select), 1) #显示窗口
main()
