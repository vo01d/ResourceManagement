#include <iostream>
#include <Windows.h>
#include <stdexcept>

class FileGuard {
public:
	FileGuard(LPCWSTR filename, DWORD desiredAccess, DWORD shareMode,
		LPSECURITY_ATTRIBUTES securityAttributes, DWORD creationDisposition,
		DWORD flagsAndAttributes, HANDLE templateFile) {
		m_fileHandle = CreateFile(filename, desiredAccess, shareMode, securityAttributes, creationDisposition, flagsAndAttributes, templateFile);
		if (m_fileHandle == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Failed to open file.");
		}
	}

	FileGuard(const FileGuard&) noexcept = delete;
	FileGuard& operator=(const FileGuard&) noexcept = delete;

	FileGuard(FileGuard&& right) noexcept : m_fileHandle(right.m_fileHandle) {
		right.m_fileHandle = INVALID_HANDLE_VALUE;
	}

	FileGuard& operator=(FileGuard&& right) noexcept {
		if (this != &right) {
			destroy();

			m_fileHandle = right.m_fileHandle;
			right.m_fileHandle = INVALID_HANDLE_VALUE;
		}

		return *this;
	}

	~FileGuard() noexcept {
		destroy();
	}

	HANDLE get() const {
		return m_fileHandle;
	}
private:
	void destroy() {
		if (m_fileHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(m_fileHandle);
		}
	}
	HANDLE m_fileHandle;
};

int main() {
	try {
		FileGuard fileGuard(TEXT("text.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		DWORD bytesWritten;
		const char* text = "Hello! How are you doing? I hope you're having a wonderful day filled with joy and success!";
		if (!WriteFile(fileGuard.get(), text, std::strlen(text), &bytesWritten, NULL)) {
			throw std::runtime_error("Failed to write to file");
		}

		std::cout << "Data written to file successfully." << std::endl;
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}