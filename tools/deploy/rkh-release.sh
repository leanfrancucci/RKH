#!/usr/bin/env bash
#

token=""
github="https://api.github.com"
branch="master"
changelog=""
draft="false"
pre="false"
probe="false"

usage()
{
    echo "Creates and publishes a release in a GitHub repository"
    echo "Usage: rkh-release -v <version>  -r <repository> "
    echo "               [-s <directory | <tar.gz>]"
    echo "               [-b <branch>] [-m <changelog file>] [-d] [-p]"
    echo "               t <token>"
    echo "Options:"
    echo "    -v: desired version (i.e. x.y.z or x.y.z-beta)"
    echo "    -r: specifies the repository info \"owner/name\". "
    echo "        For example, \"itdelsat/CIM-ARM\""
    echo "    -s: specifies the directory or tar.gz package in order to "
    echo "        publish it."
    echo "        If this option is not supplied, the repository's tarball "
    echo "        will be upload"
    echo "    -b: specifies the commitish value that determines where the Git"
    echo "        tag is created from. Default: master"
    echo "    -m: text file describing the contents of the release"
    echo "    -d: creates a draft (unpublished) release"
    echo "    -p: identifies the release as a prerelease"
    echo "    -t: personal access token"
}

gh_curl() 
{
    curl -H "Authorization: token $token" \
         -H "Accept: application/vnd.github.v3.raw" \
         $@
}

#
# Get latest release
#
# \param repo
# \usage 
#       repo="itdelsat/release-test-a"
#       get_latest $repo
#
get_latest()
{
    if [ -n $1 ]; then
        response=$(gh_curl -s $github/repos/$1/releases/latest)
        result=$(echo $response | jq "select(.message == \"Not Found\")")
        if [ -n "$result" ]; then
            echo "ERROR: cannot find latest release from $1 repository"
        else
            tag_name=$(echo $response | jq .tag_name)
            published_at=$(echo $response | jq .published_at)
            echo "Get latest release $tag_name published at $published_at"
        fi
    else
        echo "ERROR: get_latest() - missing or wrong arguments"
    fi
}

#
# Upload a file to latest release
#
# \param repo
# \param file
# \usage 
#       repo="itdelsat/release-test-a"
#       file="README.md"
#       upload_file $repo $file
#
upload_file()
{
    if [[ -n $1 && -n $2 ]]; then
        echo "Uploading file $2 to $1..."
        get_latest $1
        upload_url="$(echo "$response" | jq -r .upload_url | sed -e "s/{?name,label}//")"
        response=$(curl --silent -H "Authorization: token $token" \
                        --header "Content-Type:application/gzip" \
                        --data-binary "$2" \
                        "$upload_url?name="$2"")
        result=$(echo $response | jq "select(.message == \"Validation Failed\")")
        if [ -n "$result" ]; then
            result=$(echo $response | jq ".errors | .[0].code")
            echo "ERROR: cannot upload file $2 to $tag_name release. Code from server: $result"
        else
            echo "Upload file $2 to $tag_name release"
        fi
    else
        echo "ERROR: upload_file() - missing or wrong arguments"
    fi
}

#
# Create a release
#
# \param repo
# \param version
# \param change log file
# \usage 
#       repo="itdelsat/release-test-a"
#       version="1.0.16"
#       message="Release X"
#       create_release $repo $version "$message"
#
create_release()
{
    if [[ -n $1 && -n $2 && -n $3 ]]; then
        echo "Creating release $2 of $1..."
        payload=$(
        jq --null-input \
            --arg tag "v$2" \
            --arg body "$3" \
            --argjson prerelease $pre \
            --argjson draft $draft \
            '{tag_name: $tag, target_commitish: "master", name: $tag, body: $body, draft: $draft, prerelease: $prerelease}'
        )
        response=$(
            curl  \
                 --silent \
                 --location \
                 --data "$payload" \
                 "$github/repos/$1/releases?access_token=$token"
          )

        result=$(echo $response | jq "select(.message == \"Validation Failed\")")
        if [ -n "$result" ]; then
            result=$(echo $response | jq ".errors | .[0].code")
            echo "ERROR: cannot create the release $2. Code from server: $result"
        else
            upload_url="$(echo "$response" | jq -r .upload_url | sed -e "s/{?name,label}//")"
            echo "Release v$2 published in $1 repository"
        fi
    else
        echo "ERROR: create_release() - missing or wrong arguments"
    fi
}

#
# Get tarball from a repository
#
# \param repo
# \usage 
#       repo="itdelsat/release-test-a"
#       get_tarball $repo
#
get_tarball()
{
    if [ -n $1 ]; then
        tarball=$(basename $1).tar.gz
        echo "Getting tarball $tarball from $1/$branch..."
        curl -H "Authorization: token $token" -L $github/repos/$1/tarball/$branch > $tarball
    else
        echo "ERROR: get_tarball() - missing or wrong arguments"
    fi
}

#
# Check arguments
#
check_args()
{
    if [[ -z $version || -z $token ]]; then
        usage
        exit 1
    fi

    if [[ ! -z $changelog && -e $changelog ]]; then
        message=$(<$changelog)
    else
        message=$version
    fi
}

while getopts v:m:b:dps:r:t: option
do
    case "${option}" in
        v) version="$OPTARG";;
        m) changelog="$OPTARG";;
        b) branch="$OPTARG";;
        d) draft="true";;
        p) pre="true";;
        s) dir="$OPTARG";;
        r) repository="$OPTARG";;
        t) token="$OPTARG";;
    esac
done

check_args
if [ -z $repository ]; then
    echo "ERROR: unavailable repository info (\"owner/name\")"
    exit 1
fi

create_release $repository $version "$message"
if [ -z $dir ]; then
    get_tarball $repository
    upload_file $repository $tarball
elif [ -d $dir ]; then
    echo "Compressing $dir..."
    tar -czf $(basename $dir).tar.gz $dir
    upload_file $repository $(basename $dir).tar.gz
elif [ -e $dir ]; then
    ext="${dir##*.}"
    if [ "$ext" == "tar.gz" ]; then
        upload_file $repository $(basename $dir)
    fi
fi
exit 0
