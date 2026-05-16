plugins {
  alias(libs.plugins.android.application)
  alias(libs.plugins.kotlin.android)
}

android {
  namespace = "com.mark.client"
  compileSdk = 35
  ndkVersion = "27.0.12077973"
  
  defaultConfig {
    applicationId = "com.mark.client"
    minSdk = 21
    targetSdk = 35
    versionCode = 1
    versionName = "1.0.0"
    
    ndk {
      abiFilters += listOf("arm64-v8a", "armeabi-v7a")
    }
    
    externalNativeBuild {
      cmake {
        cppFlags += listOf("-std=c++17", "-O3")
        arguments += "-DANDROID_STL=c++_shared"
      }
    }
  }
  
  buildTypes {
    release {
      isMinifyEnabled = false
      proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
      signingConfig = signingConfigs.getByName("debug")
    }
  }
  
  externalNativeBuild {
    cmake {
      path = file("CMakeLists.txt")
      version = "3.22.1"
    }
  }
  
  compileOptions {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
  }
  
  kotlin {
    compilerOptions {
      jvmTarget.set(org.jetbrains.kotlin.gradle.dsl.JvmTarget.JVM_17)
    }
  }
}

dependencies {
  implementation(libs.androidx.appcompat)
}
