#pragma once

namespace rage
{
	// Same as in V
	struct fiFindData
	{
		char fileName[256];
		uint64_t fileSize;
		FILETIME lastWriteTime;
		DWORD fileAttributes;
	};

	class fiResourceInfo;

	class fiDevice
	{
	public:
		virtual ~fiDevice() = default;
		virtual HANDLE Open(const char* fileName, bool readOnly, uint32_t dwShareMode, uint32_t dwFlagsAndAttributes) = 0;
		virtual HANDLE OpenOverlappedWrap(const char* fileName) = 0;
		virtual HANDLE OpenOverlapped() = 0;
		virtual HANDLE OpenWrap(const char* fileName, bool readOnly) = 0; // called in VT[11]
		virtual HANDLE OpenBulk(const char* fileName, uint64_t* ptr) = 0;
		virtual HANDLE OpenBulkOverlapped() = 0;
		virtual HANDLE CreateBulk(const char* fileName) = 0;
		virtual HANDLE Create(const char* fileName, uint32_t dwShareMode, uint32_t dwFlagsAndAttributes) = 0;
		virtual HANDLE CreateWrap(const char* fileName) = 0;

		virtual uint32_t Read(HANDLE handle, void* buffer, uint32_t toRead) = 0;
		virtual int ReadFile(const char* filename, void* buffer, int size) = 0; // opens the file (VT[4]), calls Read on it (VT[10]), calls VT[21] ((Size64) bunch of seek calls (VT[16])), then closes the file (VT[18]). It returns full file size on success.
		virtual uint32_t ReadBulk(HANDLE handle, uint64_t ptr, char* buffer, uint32_t toRead) = 0;
		virtual uint32_t ReadBulkOverlapped(HANDLE handle, uint64_t ptr, char* buffer, uint32_t toRead) = 0;

		virtual uint32_t WriteBulk(HANDLE handle, uint64_t offset, const void* buffer, uint32_t length) = 0;
		virtual uint32_t Write(HANDLE handle, const void* buffer, uint32_t length) = 0;

		virtual uint32_t Seek(HANDLE handle, int32_t distance, uint32_t method) = 0; // Delegates to Seek64
		virtual uint64_t Seek64(HANDLE handle, int64_t distance, uint32_t method) = 0;

		virtual uint32_t Close(HANDLE handle) = 0;
		virtual uint32_t CloseBulk(HANDLE handle) = 0; // Same impl as Close..
		virtual uint32_t CloseBulkWrap(HANDLE handle) = 0; // Delegates to CloseBulk

		virtual uint32_t Size(HANDLE handle) = 0; // Delegates to Size64
		virtual uint64_t Size64(HANDLE handle) = 0;

		virtual uint32_t Flush() = 0; // Just returns 0

		virtual bool Delete(const char* fileName) = 0;

		virtual bool Rename(const char* from, const char* to) = 0;
		virtual bool RenameWithProgress(const char* from, const char* to) = 0;

		virtual bool MakeDirectory(const char* dir) = 0;
		virtual bool UnmakeDirectory(const char* dir) = 0;

		virtual uint64_t GetDiskFreeSpaceDir(const char* dir) = 0;

		virtual void NullSub() = 0;

		virtual uint64_t GetFileSizes(void* files, void* sizes) = 0; // probably not the best param types. First is an array of files which is iterated through and the size of each each calculated and put in the second array of sizes. The sum is returned.
		virtual uint64_t GetFileSize(const char* fileName) = 0;
		
		virtual uint64_t GetFileTimes(void* files, void* times) = 0; // Like GetFileSizes, but for times. Except that it doesn't return the sum, it returns times[0] if times[1] != 0, otherwise 0.
		virtual uint64_t GetFileTime(const char* fileName) = 0;

		virtual bool SetFileTime(const char* file, uint64_t fileTime) = 0;

		virtual HANDLE FindFileBegin(const char* path, const char* wildcard, rage::fiFindData* findData) = 0;
		virtual HANDLE FindFileBegin(const char* path, rage::fiFindData* findData) = 0;

		virtual bool FindFileNext(HANDLE handle, rage::fiFindData* findData) = 0;
		virtual int FindFileEnd(HANDLE handle) = 0;

		virtual rage::fiDevice* GetLowLevelDevice() = 0; // Returns this

		virtual void* FixRelativeName(char*, int, const char*) = 0;

		virtual bool SetEndOfFile(HANDLE handle) = 0;

		virtual uint32_t GetAttributes(const char* path) = 0;
		virtual bool SetAttributes(const char* file, uint32_t attributes) = 0;

		virtual uint64_t GetRootDeviceId(const char*) = 0; // Returns 2

		virtual uint32_t IsMemoryMappedDevice() = 0; // Returns 0

		virtual bool SafeRead(HANDLE handle, void* buffer, uint32_t length) = 0;
		virtual bool SafeWrite(HANDLE handle, const void* buffer, uint32_t length) = 0;

		virtual int GetResourceInfos(void* resources, fiResourceInfo** flags) = 0;
		virtual uint32_t GetResourceInfo(const char* fileName, fiResourceInfo* flags) = 0;


		virtual int32_t IsValidHandle() = 0; // returns 0

		virtual uint32_t GetBulkOffset(HANDLE handle) = 0; // returns 1

		virtual uint32_t ReturnZero() = 0;

		virtual uint32_t GetPhysicalSortKey(const char*) = 0;

		virtual bool IsRpf() = 0; // returns 0
		virtual uint8_t GetRpfVersion() = 0; // returns -1
		virtual fiDevice* GetRpfDevice() = 0; // returns this
		virtual bool IsCloud() = 0; // returns 0
		virtual uint64_t GetPackfileIndex() = 0; // returns 0
		virtual const char* GetDebugName() = 0;

		virtual bool SupportsOverlappedIO() = 0; // returns 0;
		virtual uint32_t GetClampedBufferSize() = 0; // returns the third param
		virtual uint64_t GetFinalOffset() = 0; // returns the second param
		virtual uint64_t IsOverlappedRequestFinished(uint64_t, int) = 0;

		virtual HANDLE CreateMapping(HANDLE fileHandle, bool writeAccess, uint64_t offset, size_t viewSize, void** outMappedView) = 0;
		virtual int CloseMapping(HANDLE mappingHandle, void* mappedView) = 0;

		bool Mount(const char* mountPoint);
		void SetPath(const char* path, bool allowRoot, rage::fiDevice* parent);
	};

	class __declspec(novtable) fiDeviceRelative
	{
		void* VMT;
		char pad[0x168];
	public:
		fiDeviceRelative();

		bool Mount(const char* mountPoint);
		void SetPath(const char* path, bool allowRoot, rage::fiDevice* parent);
	};

	class fiDeviceLocal : public fiDevice
	{
		char pad[0x180];
	public:
		fiDeviceLocal();
		~fiDeviceLocal();
		HANDLE Open(const char* fileName, bool readOnly, uint32_t dwShareMode, uint32_t dwFlagsAndAttributes) override;
		HANDLE OpenOverlappedWrap(const char* fileName) override;
		HANDLE OpenOverlapped() override;
		HANDLE OpenWrap(const char* fileName, bool readOnly) override;
		HANDLE OpenBulk(const char* fileName, uint64_t* ptr) override;
		HANDLE OpenBulkOverlapped() override;
		HANDLE CreateBulk(const char* fileName) override;
		HANDLE Create(const char* fileName, uint32_t dwShareMode, uint32_t dwFlagsAndAttributes) override;
		HANDLE CreateWrap(const char* fileName) override;

		uint32_t Read(HANDLE handle, void* buffer, uint32_t toRead) override;
		int ReadFile(const char* fileName, void* buffer, int size) override;
		uint32_t ReadBulk(HANDLE handle, uint64_t ptr, char* buffer, uint32_t toRead) override;
		uint32_t ReadBulkOverlapped(HANDLE handle, uint64_t ptr, char* buffer, uint32_t toRead) override;

		uint32_t WriteBulk(HANDLE handle, uint64_t offset, const void* buffer, uint32_t length) override;
		uint32_t Write(HANDLE handle, const void* buffer, uint32_t length) override;

		uint32_t Seek(HANDLE handle, int32_t distance, uint32_t method) override;
		uint64_t Seek64(HANDLE handle, int64_t distance, uint32_t method) override;

		uint32_t Close(HANDLE handle) override;
		uint32_t CloseBulk(HANDLE handle) override;
		uint32_t CloseBulkWrap(HANDLE handle) override;

		uint32_t Size(HANDLE handle) override;
		uint64_t Size64(HANDLE handle) override;

		uint32_t Flush() override;

		bool Delete(const char* fileName) override;

		bool Rename(const char* from, const char* to) override;
		bool RenameWithProgress(const char* from, const char* to) override;

		bool MakeDirectory(const char* dir) override;
		bool UnmakeDirectory(const char* dir) override;

		uint64_t GetDiskFreeSpaceDir(const char* dir) override;

		void NullSub() override;

		uint64_t GetFileSizes(void* files, void* sizes) override;
		uint64_t GetFileSize(const char* fileName) override;

		uint64_t GetFileTimes(void* files, void* times) override;
		uint64_t GetFileTime(const char* fileName) override;

		bool SetFileTime(const char* fileName, uint64_t fileTime) override;

		HANDLE FindFileBegin(const char* path, const char* wildcard, rage::fiFindData* findData) override;
		HANDLE FindFileBegin(const char* path, rage::fiFindData* findData) override;

		bool FindFileNext(HANDLE handle, rage::fiFindData* findData) override;
		int FindFileEnd(HANDLE handle) override;

		rage::fiDevice* GetLowLevelDevice() override;

		void* FixRelativeName(char*, int, const char*) override;

		bool SetEndOfFile(HANDLE handle) override;

		uint32_t GetAttributes(const char* path) override;
		bool SetAttributes(const char* fileName, uint32_t attributes) override;

		uint64_t GetRootDeviceId(const char*) override;

		uint32_t IsMemoryMappedDevice() override;

		bool SafeRead(HANDLE handle, void* buffer, uint32_t length) override;
		bool SafeWrite(HANDLE handle, const void* buffer, uint32_t length) override;

		int GetResourceInfos(void* resources, fiResourceInfo** flags) override;
		uint32_t GetResourceInfo(const char* fileName, fiResourceInfo* flags) override;

		int32_t IsValidHandle() override;

		uint32_t GetBulkOffset(HANDLE handle) override;

		uint32_t ReturnZero() override;

		uint32_t GetPhysicalSortKey(const char*) override;

		bool IsRpf() override;
		uint8_t GetRpfVersion() override;
		rage::fiDevice* GetRpfDevice() override;
		bool IsCloud() override;
		uint64_t GetPackfileIndex() override;
		const char* GetDebugName() override;

		bool SupportsOverlappedIO() override;
		uint32_t GetClampedBufferSize() override;
		uint64_t GetFinalOffset() override;
		uint64_t IsOverlappedRequestFinished(uint64_t, int) override;

		HANDLE CreateMapping(HANDLE fileHandle, bool writeAccess, uint64_t offset, size_t viewSize, void** outMappedView) override;
		int CloseMapping(HANDLE mappingHandle, void* mappedView) override;

		void ConvertFoundData(LPWIN32_FIND_DATAW foundDataWin, rage::fiFindData* foundDataRage);

		static std::string ToFullPath(const char* fileName)
		{
			std::filesystem::path cwd = std::filesystem::current_path() / "newmods/";

			std::string fName(fileName);

			// remove the crc part in the device name so you can use the normal respective foldeer
			auto pos = fName.find("crc:/");
			if (pos != std::string::npos)
				fName.erase(pos, 4);

			pos = fName.find(":/");
			if (pos != std::string::npos)
				fName.erase(pos, 1);

			cwd /= fName;

			cwd.make_preferred();

			return cwd.string();
		}
	};
}