/*
 *    Copyright (C) 2010 10gen Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mongo/tools/mongofiles_options.h"

#include "mongo/base/status.h"
#include "mongo/util/options_parser/startup_options.h"

namespace mongo {

    MongoFilesGlobalParams mongoFilesGlobalParams;

    Status addMongoFilesOptions(moe::OptionSection* options) {
        Status ret = addGeneralToolOptions(options);
        if (!ret.isOK()) {
            return ret;
        }

        ret = addRemoteServerToolOptions(options);
        if (!ret.isOK()) {
            return ret;
        }

        ret = addLocalServerToolOptions(options);
        if (!ret.isOK()) {
            return ret;
        }

        ret = addSpecifyDBCollectionToolOptions(options);
        if (!ret.isOK()) {
            return ret;
        }

        options->addOptionChaining("local", "local,l", moe::String,
                "local filename for put|get (default is to use the same name as "
                "'gridfs filename')");

        options->addOptionChaining("type", "type,t", moe::String,
                "MIME type for put (default is to omit)");

        options->addOptionChaining("replace", "replace,r", moe::Switch,
                "Remove other files with same name after PUT");

        options->addOptionChaining("command", "command", moe::String,
                "gridfs command to run")
                                  .hidden()
                                  .setSources(moe::SourceCommandLine)
                                  .positional(1, 1);

        options->addOptionChaining("file", "file", moe::String,
                "'gridfs filename' with a special meaning for various commands")
                                  .hidden()
                                  .setSources(moe::SourceCommandLine)
                                  .positional(2, 2);


        return Status::OK();
    }

    void printMongoFilesHelp(std::ostream* out) {
        *out << "Browse and modify a GridFS filesystem.\n" << std::endl;
        *out << "usage: mongofiles [options] command [gridfs filename]" << std::endl;
        *out << "command:" << std::endl;
        *out << "  one of (list|search|put|get)" << std::endl;
        *out << "  list - list all files.  'gridfs filename' is an optional prefix " << std::endl;
        *out << "         which listed filenames must begin with." << std::endl;
        *out << "  search - search all files. 'gridfs filename' is a substring " << std::endl;
        *out << "           which listed filenames must contain." << std::endl;
        *out << "  put - add a file with filename 'gridfs filename'" << std::endl;
        *out << "  get - get a file with filename 'gridfs filename'" << std::endl;
        *out << "  delete - delete all files with filename 'gridfs filename'" << std::endl;
        *out << moe::startupOptions.helpString();
        *out << std::flush;
    }

    bool handlePreValidationMongoFilesOptions(const moe::Environment& params) {
        if (params.count("help")) {
            printMongoFilesHelp(&std::cout);
            return false;
        }
        return true;
    }

    Status storeMongoFilesOptions(const moe::Environment& params,
                                  const std::vector<std::string>& args) {
        Status ret = storeGeneralToolOptions(params, args);
        if (!ret.isOK()) {
            return ret;
        }

        mongoFilesGlobalParams.command = getParam("command");
        mongoFilesGlobalParams.gridFSFilename = getParam("file");
        mongoFilesGlobalParams.localFile = getParam("local", mongoFilesGlobalParams.gridFSFilename);
        mongoFilesGlobalParams.contentType = getParam("type", "");
        mongoFilesGlobalParams.replace = hasParam("replace");

        return Status::OK();
    }

}
