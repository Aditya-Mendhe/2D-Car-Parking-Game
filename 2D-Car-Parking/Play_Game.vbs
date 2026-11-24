Set objShell = CreateObject("WScript.Shell")
Set objFSO = CreateObject("Scripting.FileSystemObject")
strDir = objFSO.GetParentFolderName(WScript.ScriptFullName)
strHtml = strDir & "\index.html"
strUrl = "file:///" & Replace(strHtml, "\", "/")

' Check Edge 32-bit/64-bit paths
strEdge1 = "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
strEdge2 = "C:\Program Files\Microsoft\Edge\Application\msedge.exe"
strChrome = "C:\Program Files\Google\Chrome\Application\chrome.exe"

If objFSO.FileExists(strEdge1) Then
    objShell.Run Chr(34) & strEdge1 & Chr(34) & " --app=" & Chr(34) & strUrl & Chr(34) & " --window-size=1020,800", 1, False
ElseIf objFSO.FileExists(strEdge2) Then
    objShell.Run Chr(34) & strEdge2 & Chr(34) & " --app=" & Chr(34) & strUrl & Chr(34) & " --window-size=1020,800", 1, False
ElseIf objFSO.FileExists(strChrome) Then
    objShell.Run Chr(34) & strChrome & Chr(34) & " --app=" & Chr(34) & strUrl & Chr(34) & " --window-size=1020,800", 1, False
Else
    objShell.Run Chr(34) & strHtml & Chr(34), 1, False
End If
