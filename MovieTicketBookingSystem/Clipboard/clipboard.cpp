#include <iostream>
#include <string>
#include <windows.h> // For Windows API functions


// --- Function to convert std::string (UTF-8) to std::wstring (UTF-16) ---
std::wstring s2ws(const std::string& str) {
    if (str.empty()) {
        return L"";
    }
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// --- Function to convert std::wstring (UTF-16) to std::string (UTF-8) ---
std::string ws2s(const std::wstring& wstr) {
    if (wstr.empty()) {
        return "";
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}


bool writeToClipboard(const std::string& text) {
    // Convert UTF-8 std::string to UTF-16 std::wstring for Windows clipboard
    std::wstring wtext = s2ws(text);

    // Open the clipboard
    if (!OpenClipboard(NULL)) {
        return false;
    }

    // Empty the clipboard
    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    // Allocate global memory for the text
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (wtext.length() + 1) * sizeof(wchar_t));
    if (hGlobal == NULL) {
        CloseClipboard();
        return false;
    }

    // Lock the global memory to get a pointer to it
    LPWSTR lpwcstr = static_cast<LPWSTR>(GlobalLock(hGlobal));
    if (lpwcstr == NULL) {
        GlobalFree(hGlobal); // Free memory if lock fails
        CloseClipboard();
        return false;
    }

    // Copy the wide string into the allocated global memory
    wcscpy_s(lpwcstr, wtext.length() + 1, wtext.c_str());

    // Unlock the global memory
    GlobalUnlock(hGlobal);

    // 7. Place the data onto the clipboard
    if (!SetClipboardData(CF_UNICODETEXT, hGlobal)) {
        GlobalFree(hGlobal);
        CloseClipboard();
        return false;
    }

    // Close the clipboard
    CloseClipboard();

	Sleep(100); // Optional: Sleep to ensure clipboard is ready for next operation
    return true;
}


std::string readFromClipboard() {
    std::string text = "";

    // Open the clipboard
    if (!OpenClipboard(NULL)) {
        throw std::runtime_error("");
        return "";
    }

    // Check if text data is available (CF_UNICODETEXT is preferred for Unicode)
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        // Fallback to CF_TEXT (ANSI) if Unicode is not available
        if (!IsClipboardFormatAvailable(CF_TEXT)) {
            throw std::runtime_error("");
            CloseClipboard();
            return "";
        }
    }

    // 3. Get the handle to the clipboard data
    HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT);
    if (hGlobal == NULL) {
        // If CF_UNICODETEXT failed, try CF_TEXT
        hGlobal = GetClipboardData(CF_TEXT);
        if (hGlobal == NULL) {
            throw std::runtime_error("");
            CloseClipboard();
            return "";
        }
        // If we got CF_TEXT, process it as ANSI
        LPSTR lpstr = static_cast<LPSTR>(GlobalLock(hGlobal));
        if (lpstr != NULL) {
            text = lpstr;
            GlobalUnlock(hGlobal);
        }
    }
    else {
        // Process CF_UNICODETEXT
        LPWSTR lpwcstr = static_cast<LPWSTR>(GlobalLock(hGlobal));
        if (lpwcstr != NULL) {
            text = ws2s(lpwcstr); // Convert UTF-16 to UTF-8
            GlobalUnlock(hGlobal);
        }
    }

    // Close the clipboard
    CloseClipboard();
    return text;
}