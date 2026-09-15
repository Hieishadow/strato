/*
 * SPDX-License-Identifier: MPL-2.0
 * Copyright © 2022 Skyline Team and Contributors (https://github.com/skyline-emu/)
 */

package org.stratoemu.strato.utils

import android.content.Context
import android.os.Build
import android.util.Log
import org.stratoemu.strato.R
import org.stratoemu.strato.data.GpuDriverMetadata
import org.stratoemu.strato.getPublicFilesDir
import kotlinx.serialization.SerializationException
import java.io.File
import java.io.IOException
import java.io.InputStream

private const val GPU_DRIVER_DIRECTORY = "gpu_drivers"
private const val GPU_DRIVER_FILE_REDIRECT_DIR = "gpu/vk_file_redirect"
private const val GPU_DRIVER_INSTALL_TEMP_DIR = "driver_temp"
private const val GPU_DRIVER_META_FILE = "meta.json"
private const val TAG = "GPUDriverHelper"

interface GpuDriverHelper {
    companion object {
        private external fun getSystemDriverInfo() : Array<String>?
        external fun supportsCustomDriverLoading() : Boolean
        external fun supportsForceMaxGpuClocks() : Boolean
        external fun forceMaxGpuClocks(enable : Boolean)

        fun getInstalledDrivers(context : Context) : Map<File, GpuDriverMetadata> {
            val gpuDriverDir = getDriversDirectory(context)
            val driverMap = mutableMapOf<File, GpuDriverMetadata>()
            gpuDriverDir.listFiles()?.forEach { entry ->
                if (!entry.isDirectory) {
                    entry.delete()
                    return@forEach
                }
                val metadataFile = File(entry.canonicalPath, GPU_DRIVER_META_FILE)
                if (!metadataFile.exists()) {
                    entry.delete()
                    return@forEach
                }
                try {
                    driverMap[entry] = GpuDriverMetadata.deserialize(metadataFile)
                } catch (e : SerializationException) {
                    Log.w(TAG, "Failed to load gpu driver metadata for ${entry.name}, skipping\n${e.message}")
                }
            }
            return driverMap
        }

        fun getSystemDriverMetadata(context : Context) : GpuDriverMetadata {
            val systemDriverInfo = getSystemDriverInfo()
            return GpuDriverMetadata(
                name = context.getString(R.string.system_driver),
                author = "",
                packageVersion = "",
                vendor = systemDriverInfo?.get(0)?: "",
                driverVersion = systemDriverInfo?.get(1)?: "",
                minApi = 0,
                description = context.getString(R.string.system_driver_desc),
                libraryName = ""
            )
        }

        fun installDriver(context : Context, stream : InputStream) : GpuDriverInstallResult {
            val installTempDir = File(context.cacheDir.canonicalPath, GPU_DRIVER_INSTALL_TEMP_DIR).apply {
                deleteRecursively()
            }
            try {
                ZipUtils.unzip(stream, installTempDir)
            } catch (e : Exception) {
                e.printStackTrace()
                installTempDir.deleteRecursively()
                return GpuDriverInstallResult.InvalidArchive
            }
            return installUnpackedDriver(context, installTempDir)
        }

        fun installDriver(context : Context, file : File) : GpuDriverInstallResult {
            val installTempDir = File(context.cacheDir.canonicalPath, GPU_DRIVER_INSTALL_TEMP_DIR).apply {
                deleteRecursively()
            }
            try {
                ZipUtils.unzip(file, installTempDir)
            } catch (e : Exception) {
                e.printStackTrace()
                installTempDir.deleteRecursively()
                return GpuDriverInstallResult.InvalidArchive
            }
            return installUnpackedDriver(context, installTempDir)
        }

        private fun installUnpackedDriver(context : Context, unpackDir : File) : GpuDriverInstallResult {
            val cleanup = {
                unpackDir.deleteRecursively()
            }
            val metadataFile = File(unpackDir, GPU_DRIVER_META_FILE)
            if (!metadataFile.isFile) {
                cleanup()
                return GpuDriverInstallResult.MissingMetadata
            }
            val driverMetadata = try {
                GpuDriverMetadata.deserialize(metadataFile)
            } catch (e : SerializationException) {
                cleanup()
                return GpuDriverInstallResult.InvalidMetadata
            }
            if (Build.VERSION.SDK_INT < driverMetadata.minApi) {
                cleanup()
                return GpuDriverInstallResult.UnsupportedAndroidVersion
            }
            val installedDrivers = getInstalledDrivers(context)
            val finalInstallDir = File(getDriversDirectory(context), driverMetadata.label)
            if (installedDrivers[finalInstallDir]!= null) {
                finalInstallDir.deleteRecursively()
            }
            if (!unpackDir.renameTo(finalInstallDir)) {
                try {
                    unpackDir.copyRecursively(finalInstallDir, overwrite = true)
                    unpackDir.deleteRecursively()
                } catch (e: Exception) {
                    cleanup()
                    throw IOException("Failed to create directory ${finalInstallDir.name}: ${e.message}")
                }
            }
            return GpuDriverInstallResult.Success
        }

        // BUILD 16 - FIX ADRENO 650 S20 FE
        fun getLibraryName(context : Context, driverLabel : String) : String {
            val driverDir = File(getDriversDirectory(context), driverLabel)
            val metadataFile = File(driverDir, GPU_DRIVER_META_FILE)
            try {
                val meta = GpuDriverMetadata.deserialize(metadataFile)
                if (meta.libraryName.isNotEmpty()) {
                    val exists = driverDir.walkTopDown().any { it.name == meta.libraryName }
                    if (exists) {
                        Log.i(TAG, "Usando library do meta.json: ${meta.libraryName}")
                        return meta.libraryName
                    }
                    return meta.libraryName
                }
            } catch (e : Exception) {
                Log.w(TAG, "meta.json falhou, buscando.so recursivo: ${e.message}")
            }

            // Busca recursiva que corrige Turnip R18/R19 no 865
            val soFile = driverDir.walkTopDown().firstOrNull { it.isFile && it.extension == "so" }
            if (soFile!= null) {
                Log.i(TAG, "Driver.so encontrado: ${soFile.name}")
                return soFile.name
            }

            Log.e(TAG, "Nenhum.so encontrado em $driverLabel")
            return ""
        }

        fun ensureFileRedirectDir(context : Context) {
            File(context.getPublicFilesDir(), GPU_DRIVER_FILE_REDIRECT_DIR).apply {
                if (!isDirectory) {
                    delete()
                    mkdirs()
                }
            }
        }

        private fun getDriversDirectory(context : Context) = File(context.filesDir.canonicalPath, GPU_DRIVER_DIRECTORY).apply {
            if (!isDirectory) {
                delete()
                mkdirs()
            }
        }
    }
}

enum class GpuDriverInstallResult {
    Success,
    InvalidArchive,
    MissingMetadata,
    InvalidMetadata,
    UnsupportedAndroidVersion,
    AlreadyInstalled,
}
