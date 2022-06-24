#pragma once
#include <list>
#include "Album.h"
#include "User.h"
#include "sqlite3.h"

class IDataAccess
{
public:
	virtual ~IDataAccess() = default;

	// album related done
	virtual const std::list<Album> getAlbumsOfUser(const User& user) = 0;//done
	virtual const std::list<Album> getAlbums() = 0;//done
	virtual void createAlbum(const Album& album) = 0;//done
	virtual void deleteAlbum(const std::string& albumName, int userId) = 0;//done
	virtual bool doesAlbumExists(const std::string& albumName, int userId) = 0;//done
	virtual Album openAlbum(const std::string& albumName) = 0;//done
	virtual void closeAlbum(Album& pAlbum) = 0;//done
	virtual void printAlbums() = 0;//done

    // picture related done
	virtual void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) = 0;//done
	virtual void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) = 0;//done
	virtual void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) = 0;//done
	virtual void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) = 0;//done

	// user related done
	virtual void printUsers() =0;//done
	virtual User getUser(int userId) = 0;//done
	virtual void createUser(User& user ) = 0;//done
	virtual void deleteUser(const User& user) = 0;//done
	virtual bool doesUserExists(int userId) = 0 ;//done
	

	// user statistics
	virtual int countAlbumsOwnedOfUser(const User& user) = 0;//done
	virtual int countTagsOfUser(const User& user) = 0;//done

	virtual int countAlbumsTaggedOfUser(const User& user) = 0;
	virtual float averageTagsPerAlbumOfUser(const User& user) = 0;

	// queries done
	virtual User getTopTaggedUser() = 0;//done
	virtual Picture getTopTaggedPicture() = 0;//done
	virtual std::list<Picture> getTaggedPicturesOfUser(const User& user) = 0;//done
	
	//done
	virtual bool open() = 0;
	virtual void close() = 0;
	virtual void clear() = 0;
};
