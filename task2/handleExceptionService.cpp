#pragma once
#include <iostream>
#include <string>



using namespace std;

bool isValidParcelID( const string& parcelID){
    if(parcelID.empty()){
        cout << "Error: Parcel ID cannot be empty." << endl;
        return false;
    }
    else{
        cout << "Parcel ID is valid." << endl;
        return true;
    }
}

