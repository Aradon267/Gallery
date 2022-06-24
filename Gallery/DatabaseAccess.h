#pragma once
#include "IDataAccess.h"
#include <iostream>
#include <io.h>
#include <typeinfo>

using namespace std;

class DatabaseAccess : public IDataAccess
{
public:
	DatabaseAccess() = default;
	virtual ~DatabaseAccess() = default;

	virtual bool open();
	virtual void close();
	virtual void clear();

	virtual const std::list<Album> getAlbumsOfUser(const User& user);
	virtual const std::list<Album> getAlbums();
	virtual Album openAlbum(const std::string& albumName);
	virtual void createAlbum(const Album& album);
	virtual void closeAlbum(Album& pAlbum);
	virtual void deleteAlbum(const std::string& albumName, int userId);
	virtual bool doesAlbumExists(const std::string& albumName, int userId);
	virtual void printAlbums();

	virtual void createUser(User& user);
	virtual void deleteUser(const User& user);
	virtual void printUsers();
	virtual bool doesUserExists(int userId);
	virtual User getUser(int userId);

	virtual void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId);
	virtual void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId);
	virtual void addPictureToAlbumByName(const std::string& albumName, const Picture& picture);
	virtual void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName);

	virtual int countAlbumsTaggedOfUser(const User& user);
	virtual float averageTagsPerAlbumOfUser(const User& user);

	virtual int countAlbumsOwnedOfUser(const User& user);
	virtual int countTagsOfUser(const User& user);

	virtual User getTopTaggedUser();
	virtual Picture getTopTaggedPicture();
	virtual std::list<Picture> getTaggedPicturesOfUser(const User& user);

	static int callbackPrints(void* data, int argc, char** argv, char** azColName);
	static int callbackUser(void* data, int argc, char** argv, char** azColName);
	static int callbackPic(void* data, int argc, char** argv, char** azColName);
	static int callbackAlb(void* data, int argc, char** argv, char** azColName);
	static int callCounter(void* data, int argc, char** argv, char** azColName);

	static sqlite3* db;
};

