#!/bin/bash
echo "FallingSandSurvival project setup"
echo ""
echo "You should have already done the required steps before running this file."
echo "Please do these first unless you know what you're doing."
echo "Instructions: https://github.com/PieKing1215/FallingSandSurvival/wiki/Building"
echo ""
read -p "Press [Enter] to start (or Ctrl+C to exit)..."
echo ""

echo "Looking for Conan..."
command -v conan >/dev/null 2>&1 || { 
    echo "Conan is an open source C++ package manager, and is needed to set up libraries for FallingSandSurvival."
    echo "Run the Conan installer which you can download at: https://conan.io/downloads.html"
    echo "More instructions: https://docs.conan.io/en/latest/installation.html"
    read -p "Press [Enter] once Conan is installed to continue..."
}

command -v conan >/dev/null 2>&1 || {
    echo "Could not find the \"conan\" command.";
    echo "Maybe the install didn't finish or it wasn't added to the path?";
    echo "Try closing and reopening this console to refresh the PATH if the Conan installer said it was successful.";
    exit 1; 
}

conan --version
echo "Conan is installed."
echo "Setting up Conan remotes..."

if conan remote list | grep bincrafters -q
then
    if conan remote list | grep bincrafters.jfrog.io -q
    then
        echo "Bincrafters Conan repo is already installed."
    else
        echo "Bincrafters Conan repo moved, switching in conan..."
        conan remote remove bincrafters
        conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
        conan config set general.revisions_enabled=1
        echo "Clearing cache for sdl2_ttf..."
        conan remove sdl2_ttf -f
        echo "Done."
    fi
else
    echo "Adding Bincrafters Conan repo..."
    conan remote add bincrafters "https://bincrafters.jfrog.io/artifactory/api/conan/public-conan"
    conan config set general.revisions_enabled=1
    echo "Done."
fi

echo "Setting up Conan sdl_gpu..."
if conan search sdl_gpu | grep sdl_gpu/ -q
then
    if conan search sdl_gpu | grep sdl_gpu/20201002 -q
    then
        echo "There is a new version of sdl_gpu which needs to be installed."
        
        echo "Cloning github.com/sixten-hilborn/conan-sdl_gpu..."
        git clone https://github.com/sixten-hilborn/conan-sdl_gpu ./conan-sdl_gpu
        cd ./conan-sdl_gpu
        
        # Patches to use newer version of sdl_gpu
        sed -i 's/20171229/20201002/g' ./conanfile.py
        sed -i 's/143f767adf7d472f81ce890d4692ed29369aa8f3/47a3e2b2a9326c33ad6f177794705987399de8cf/g' ./conanfile.py
        sed -i 's/2.0.9/2.0.12/g' ./conanfile.py
        sed -i "s/sdl2 sdl2 sdl-2.0'/SDL2d sdl2 sdl2 sdl-2.0'/g" ./conanfile.py
        sed -i "s/sdl2 sdl2 sdl-2.0 SDL2d'/SDL2d sdl2 sdl2 sdl-2.0'/g" ./conanfile.py
        sed -i "s/SDL_gpu_INSTALL/INSTALL_LIBRARY/g" ./conanfile.py
        sed -i "s/SDL_gpu_BUILD_DEMOS/BUILD_DEMOS/g" ./conanfile.py
        sed -i "s/SDL_gpu_BUILD_SHARED/BUILD_SHARED/g" ./conanfile.py
        sed -i "s/SDL_gpu_BUILD_STATIC/BUILD_STATIC/g" ./conanfile.py
        sed -i "s/SDL_gpu_USE_SDL1/USE_SDL1/g" ./conanfile.py
        sed -i "s/SDL_gpu_DISABLE_GLES_1/DISABLE_GLES_1/g" ./conanfile.py
        conan export . sdl_gpu/20201002
        cd ../
        
        if conan search sdl_gpu | grep sdl_gpu/20201002 -q
        then
            echo "Successfully installed sdl_gpu into Conan."
            
            rm -rf ./conan-sdl_gpu/
        else
            echo "Failed to install sdl_gpu into Conan."
            exit 1; 
        fi
    else
        echo "Conan sdl_gpu already installed."
    fi
else
    echo "Cloning github.com/sixten-hilborn/conan-sdl_gpu..."
    git clone https://github.com/sixten-hilborn/conan-sdl_gpu ./conan-sdl_gpu
    cd ./conan-sdl_gpu
    
    # Patches to use newer version of sdl_gpu
    sed -i 's/20171229/20201002/g' ./conanfile.py
    sed -i 's/143f767adf7d472f81ce890d4692ed29369aa8f3/47a3e2b2a9326c33ad6f177794705987399de8cf/g' ./conanfile.py
    sed -i 's/2.0.9/2.0.12/g' ./conanfile.py
    sed -i "s/sdl2 sdl2 sdl-2.0'/SDL2d sdl2 sdl2 sdl-2.0'/g" ./conanfile.py
    sed -i "s/sdl2 sdl2 sdl-2.0 SDL2d'/SDL2d sdl2 sdl2 sdl-2.0'/g" ./conanfile.py
    sed -i "s/SDL_gpu_INSTALL/INSTALL_LIBRARY/g" ./conanfile.py
    sed -i "s/SDL_gpu_BUILD_DEMOS/BUILD_DEMOS/g" ./conanfile.py
    sed -i "s/SDL_gpu_BUILD_SHARED/BUILD_SHARED/g" ./conanfile.py
    sed -i "s/SDL_gpu_BUILD_STATIC/BUILD_STATIC/g" ./conanfile.py
    sed -i "s/SDL_gpu_USE_SDL1/USE_SDL1/g" ./conanfile.py
    sed -i "s/SDL_gpu_DISABLE_GLES_1/DISABLE_GLES_1/g" ./conanfile.py
    conan export . sdl_gpu/20201002
    cd ../
    
    if conan search sdl_gpu | grep sdl_gpu/20201002 -q
    then
        echo "Successfully installed sdl_gpu into Conan."
        
        rm -rf ./conan-sdl_gpu/
    else
        echo "Failed to install sdl_gpu into Conan."
        exit 1; 
    fi
    
fi

echo ""
echo "Conan should be ready to go."
echo "Make sure you install the Conan Extension for Visual Studio: https://marketplace.visualstudio.com/items?itemName=conan-io.conan-vs-extension".
echo ""
read -p "Press [Enter] to continue to the next step..."
echo ""


# if [ -d "./FallingSandSurvival/lib/FMOD/inc" ] && [ -d "./FallingSandSurvival/lib/FMOD_studio/inc" ] && [ -f "./FallingSandSurvival/lib/bin/x64/fmod.dll" ]
# then
    # echo "FMOD already set up."
# else
    # echo "Setting up FMOD (required):"
    # echo "Go to https://fmod.com/download (you will need to make an account)"
    # echo "Under \"FMOD Studio Suite\"->\"FMOD Engine\"->\"Windows\"->Download"
    # echo "This should get you an installer, so install the FMOD API files (you can uninstall later)."
    # echo ""
    # echo "Go to wherever you installed it to (by default \"C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Studio API Windows\\\")"
    # echo "Copy the api folder and paste it next to this script (it will be deleted after)."
    # echo ""
    # read -p "Press [Enter] once the api/ folder is placed next to this script..."
    
    # if [ -d "./api" ]
    # then
        # echo "./api/ found, copying files to the right places..."
        # echo "./api/core/inc -> ./FallingSandSurvival/lib/FMOD/inc"
        # cp -r ./api/core/inc ./FallingSandSurvival/lib/FMOD/inc
        # echo "./api/core/lib -> ./FallingSandSurvival/lib/FMOD/lib"
        # cp -r ./api/core/lib ./FallingSandSurvival/lib/FMOD/lib
        
        # echo "./FallingSandSurvival/lib/FMOD/lib/x86/fmod.dll -> ./FallingSandSurvival/lib/bin/x86/fmod.dll"
        # cp ./FallingSandSurvival/lib/FMOD/lib/x86/fmod.dll ./FallingSandSurvival/lib/bin/x86/fmod.dll
        # echo "./FallingSandSurvival/lib/FMOD/lib/x86/fmodL.dll -> ./FallingSandSurvival/lib/bin/x86/fmodL.dll"
        # cp ./FallingSandSurvival/lib/FMOD/lib/x86/fmodL.dll ./FallingSandSurvival/lib/bin/x86/fmodL.dll
        
        # echo "./FallingSandSurvival/lib/FMOD/lib/x64/fmod.dll -> ./FallingSandSurvival/lib/bin/x64/fmod.dll"
        # cp ./FallingSandSurvival/lib/FMOD/lib/x64/fmod.dll ./FallingSandSurvival/lib/bin/x64/fmod.dll
        # echo "./FallingSandSurvival/lib/FMOD/lib/x64/fmodL.dll -> ./FallingSandSurvival/lib/bin/x64/fmodL.dll"
        # cp ./FallingSandSurvival/lib/FMOD/lib/x64/fmodL.dll ./FallingSandSurvival/lib/bin/x64/fmodL.dll
        
        # echo "./api/studio/inc -> ./FallingSandSurvival/lib/FMOD_studio/inc"
        # cp -r ./api/studio/inc ./FallingSandSurvival/lib/FMOD_studio/inc
        # echo "./api/studio/lib -> ./FallingSandSurvival/lib/FMOD_studio/lib"
        # cp -r ./api/studio/lib ./FallingSandSurvival/lib/FMOD_studio/lib
        
        # echo "./FallingSandSurvival/lib/FMOD_studio/lib/x86/fmodstudio.dll -> ./FallingSandSurvival/lib/bin/x86/fmodstudio.dll"
        # cp ./FallingSandSurvival/lib/FMOD_studio/lib/x86/fmodstudio.dll ./FallingSandSurvival/lib/bin/x86/fmodstudio.dll
        # echo "./FallingSandSurvival/lib/FMOD_studio/lib/x86/fmodstudioL.dll -> ./FallingSandSurvival/lib/bin/x86/fmodstudioL.dll"
        # cp ./FallingSandSurvival/lib/FMOD_studio/lib/x86/fmodstudioL.dll ./FallingSandSurvival/lib/bin/x86/fmodstudioL.dll
        
        # echo "./FallingSandSurvival/lib/FMOD_studio/lib/x64/fmodstudio.dll -> ./FallingSandSurvival/lib/bin/x64/fmodstudio.dll"
        # cp ./FallingSandSurvival/lib/FMOD_studio/lib/x64/fmodstudio.dll ./FallingSandSurvival/lib/bin/x64/fmodstudio.dll
        # echo "./FallingSandSurvival/lib/FMOD_studio/lib/x64/fmodstudioL.dll -> ./FallingSandSurvival/lib/bin/x64/fmodstudioL.dll"
        # cp ./FallingSandSurvival/lib/FMOD_studio/lib/x64/fmodstudioL.dll ./FallingSandSurvival/lib/bin/x64/fmodstudioL.dll
        
        # echo "Done."
        
        # echo "Deleting ./api/ ..."
        # rm -rf ./api
        # echo "Done."
    # else
        # echo "./api/ not found, exiting..."
        # exit 1; 
    # fi
# fi

echo ""
if [ -d "./FallingSandSurvival/lib/discord_game_sdk/cpp" ] && [ -f "./FallingSandSurvival/lib/bin/x64/discord_game_sdk.dll" ]
then
    echo "Discord Game SDK already set up."
else
    while true; do
        read -p "Do you want to set up the Discord Game SDK (optional) [yes/no]? " yn
        case $yn in
            [Yy]* ) 
                echo "Downloading Discord Game SDK..."
                curl https://dl-game-sdk.discordapp.net/2.5.6/discord_game_sdk.zip -o discord_game_sdk.zip
                echo "Unzipping to ./discord_game_sdk ..."
                unzip discord_game_sdk.zip -d ./discord_game_sdk
                
                echo "Copying files to the right places..."
                echo "./discord_game_sdk/cpp -> ./FallingSandSurvival/lib/discord_game_sdk/cpp"
                cp -r ./discord_game_sdk/cpp ./FallingSandSurvival/lib/discord_game_sdk/cpp
                echo "./discord_game_sdk/lib -> ./FallingSandSurvival/lib/discord_game_sdk/lib"
                cp -r ./discord_game_sdk/lib ./FallingSandSurvival/lib/discord_game_sdk/lib
                
                echo "./FallingSandSurvival/lib/discord_game_sdk/lib/x86/discord_game_sdk.dll -> ./FallingSandSurvival/lib/bin/x86/discord_game_sdk.dll"
                cp ./FallingSandSurvival/lib/discord_game_sdk/lib/x86/discord_game_sdk.dll ./FallingSandSurvival/lib/bin/x86/discord_game_sdk.dll
                
                echo "./FallingSandSurvival/lib/discord_game_sdk/lib/x86_64/discord_game_sdk.dll -> ./FallingSandSurvival/lib/bin/x64/discord_game_sdk.dll"
                cp ./FallingSandSurvival/lib/discord_game_sdk/lib/x86_64/discord_game_sdk.dll ./FallingSandSurvival/lib/bin/x64/discord_game_sdk.dll
                
                echo "Done."
                
                echo "Setting BUILD_WITH_DISCORD to 1..."
                sed -i 's/BUILD_WITH_DISCORD 0/BUILD_WITH_DISCORD 1/g' ./FallingSandSurvival/stdafx.h
                echo "Done."
                
                echo "Cleaning up..."
                rm -rf ./discord_game_sdk
                rm ./discord_game_sdk.zip
                echo "Done."
                
                break;;
            [Nn]* ) 
                echo "Setting BUILD_WITH_DISCORD to 0..."
                sed -i 's/BUILD_WITH_DISCORD 1/BUILD_WITH_DISCORD 0/g' ./FallingSandSurvival/stdafx.h
                echo "Done."
            break;;
            * ) echo "Please enter yes or no.";;
        esac
    done
fi

echo ""
if [ -d "./FallingSandSurvival/lib/steam/include" ] && [ -f "./FallingSandSurvival/lib/bin/x64/steam_api64.dll" ]
then
    echo "Steam API already set up."
else
    while true; do
        read -p "Do you want to set up Steam API (optional) [yes/no]? " yn
        case $yn in
            [Yy]* ) 
                echo "Setting up Steam API:"
                echo "Download https://partner.steamgames.com/downloads/steamworks_sdk.zip (you will need to sign in with your Steam account)"
                echo "Place steamworks_sdk_###.zip next to this script (it will be deleted after)."
                echo ""
                read -p "Press [Enter] once steamworks_sdk_###.zip is placed next to this script..."
                
                if ls ./steamworks_sdk*.zip 1> /dev/null 2>&1
                then
                    echo "Unzipping to ./steamworks ..."
                    unzip ./steamworks_sdk*.zip -d ./steamworks_sdk
                    
                    echo "Copying files to the right places..."
                    
                    echo "./steamworks_sdk/sdk/redistributable_bin -> ./FallingSandSurvival/lib/steam/redistributable_bin"
                    cp -r ./steamworks_sdk/sdk/redistributable_bin ./FallingSandSurvival/lib/steam/redistributable_bin
                    
                    echo "./FallingSandSurvival/lib/steam/redistributable_bin/steam_api.dll -> ./FallingSandSurvival/lib/bin/x86/steam_api.dll"
                    cp ./FallingSandSurvival/lib/steam/redistributable_bin/steam_api.dll ./FallingSandSurvival/lib/bin/x86/steam_api.dll
                    
                    echo "./FallingSandSurvival/lib/steam/redistributable_bin/win64/steam_api64.dll -> ./FallingSandSurvival/lib/bin/x64/steam_api64.dll"
                    cp ./FallingSandSurvival/lib/steam/redistributable_bin/win64/steam_api64.dll ./FallingSandSurvival/lib/bin/x64/steam_api64.dll
                    
                    echo "./steamworks_sdk/sdk/public/steam -> ./FallingSandSurvival/lib/steam/include"
                    cp -r ./steamworks_sdk/sdk/public/steam ./FallingSandSurvival/lib/steam/include
                    echo "Done."
                    
                    echo "Setting BUILD_WITH_STEAM to 1..."
                    sed -i 's/BUILD_WITH_STEAM 0/BUILD_WITH_STEAM 1/g' ./FallingSandSurvival/stdafx.h
                    echo "Done."
                    
                    echo "Cleaning up..."
                    rm -rf ./steamworks_sdk
                    rm ./steamworks_sdk*.zip
                    echo "Done."
                    
                else
                    echo "./steamworks_sdk*.zip not found, exiting..."
                    exit 1; 
                fi
                break;;
            [Nn]* ) 
                echo "Setting BUILD_WITH_STEAM to 0..."
                sed -i 's/BUILD_WITH_STEAM 1/BUILD_WITH_STEAM 0/g' ./FallingSandSurvival/stdafx.h
                echo "Done."
            break;;
            * ) echo "Please enter yes or no.";;
        esac
    done
fi

echo ""
echo "== Setup complete! =="
echo "Continue to the next step"
