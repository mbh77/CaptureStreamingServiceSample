#include "pch.h"
#include "GstService.h"
#include <stdio.h>
#include <Windows.h>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <iostream>
#include <sstream>

namespace CaptureStreamingLIb
{
    namespace Gst
    {
        GstService::GstService()
        {
            m_pBus = nullptr;
            m_pMsg = nullptr;
            m_pSource = nullptr;
            m_pPipeline = nullptr;
        }

        GstService::~GstService()
        {

        }

        bool GstService::Initialize()
        {
            m_state = StreamingState::Initializing;
            gst_init(nullptr, nullptr);
            m_state = StreamingState::Ready;
            status = "Initialized";
            return true;
        }

        void GstService::StartStream()
        {
            if (m_state == StreamingState::Ready)
            {
                asyncTask = std::async(std::launch::async, [this]
                    {
                        GstStateChangeReturn ret;

                        std::ostringstream pipelineSs;

                        pipelineSs << "videotestsrc name=source pattern=smpte ";
                        pipelineSs << "! videoconvert ";
                        pipelineSs << "! autovideosink ";

                        std::string pipelineDesc = pipelineSs.str();

                        GError* error = nullptr;
                        m_pPipeline = gst_parse_launch(pipelineDesc.c_str(), &error);

                        if (!m_pPipeline) {
                            //std::cerr << "Failed to create pipeline: " << (error ? error->message : "unknown error") << "\n";
                            //if (error) g_error_free(error);
                            return;
                        }

                        m_pSource = gst_bin_get_by_name(GST_BIN(m_pPipeline), "source");
                        if (!m_pSource) {
                            std::cerr << "Failed to get videotestsrc element.\n";
                            gst_object_unref(m_pPipeline);
                            return;
                        }

                        g_object_set(G_OBJECT(m_pSource), "pattern", 0, nullptr); // 0: SMPTE pattern
                        gst_object_unref(m_pSource);

                        ret = gst_element_set_state(m_pPipeline, GST_STATE_PLAYING);
                        if (ret == GST_STATE_CHANGE_FAILURE) {
                            std::cerr << "Failed to set pipeline to playing state.\n";
                            gst_object_unref(m_pPipeline);
                            return;
                        }

                        m_pBus = gst_element_get_bus(m_pPipeline);
                        m_state = StreamingState::Streaming;
                    });
            }
        }

        void GstService::StartStream(winrt::Windows::Graphics::SizeInt32 streamFrameSize)
        {
            if (m_state == StreamingState::Ready)
            {
                frameSize = streamFrameSize;
                asyncTask = std::async(std::launch::async, [this](winrt::Windows::Graphics::SizeInt32 size)
                    {
                        GstStateChangeReturn ret;

                        std::ostringstream pipelineSs;

                        pipelineSs << "appsrc name=source ";
                        pipelineSs << "! autovideosink sync=false";

                        std::string pipelineDesc = pipelineSs.str();

                        GError* error = nullptr;
                        m_pPipeline = gst_parse_launch(pipelineDesc.c_str(), &error);

                        if (!m_pPipeline) {
                            std::cerr << "Failed to create pipeline: " << (error ? error->message : "unknown error") << "\n";
                            return;
                        }

                        m_pSource = gst_bin_get_by_name(GST_BIN(m_pPipeline), "source");
                        if (!m_pSource) {
                            std::cerr << "Failed to get appsrc element.\n";
                            return;
                        }

#ifdef USE_NV12_COMPUTE_SHADER
                        GstCaps* caps = gst_caps_new_simple(
                            "video/x-raw",
                            "format", G_TYPE_STRING, "NV12",
                            "width", G_TYPE_INT, size.Width,
                            "height", G_TYPE_INT, size.Height,
                            "framerate", GST_TYPE_FRACTION, 60, 1,
                            nullptr);
#else
                        GstCaps* caps = gst_caps_new_simple(
                            "video/x-raw",
                            "format", G_TYPE_STRING, "I420",
                            "width", G_TYPE_INT, size.Width,
                            "height", G_TYPE_INT, size.Height,
                            "framerate", GST_TYPE_FRACTION, 60, 1,
                            nullptr);
#endif

                        g_object_set(G_OBJECT(m_pSource), "caps", caps, nullptr);
                        gst_caps_unref(caps);

                        g_object_set(G_OBJECT(m_pSource), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

                        ret = gst_element_set_state(m_pPipeline, GST_STATE_PLAYING);
                        if (ret == GST_STATE_CHANGE_FAILURE) {
                            std::cerr << "Failed to set pipeline to playing state.\n";
                            return;
                        }

                        m_pBus = gst_element_get_bus(m_pPipeline);
                        m_state = StreamingState::Streaming;
                    }, streamFrameSize);
            }
        }

        void GstService::StartUdpStream(winrt::Windows::Graphics::SizeInt32 streamFrameSize)
        {
            if (m_state == StreamingState::Ready)
            {
                frameSize = streamFrameSize;
                asyncTask = std::async(std::launch::async, [this](winrt::Windows::Graphics::SizeInt32 size)
                    {
                        GstStateChangeReturn ret;

                        std::ostringstream pipelineSs;

                        pipelineSs << "appsrc name=source is-live=true format=time ";
                        pipelineSs << "! queue leaky=downstream max-size-buffers=3 max-size-bytes=0 max-size-time=0 ";
                        pipelineSs << "! mfh264enc ";
                        pipelineSs << "bitrate=6000 rc-mode=3 quality-vs-speed=67 low-latency=true ";
                        pipelineSs << "cabac=false ";
                        pipelineSs << "! queue ";
                        pipelineSs << "! rtph264pay ";
                        pipelineSs << "! rtph264depay ";
                        pipelineSs << "! h264parse ";
                        pipelineSs << "! d3d11h264dec ";
                        pipelineSs << "! autovideosink sync=false ";

                        std::string pipelineDesc = pipelineSs.str();

                        GError* error = nullptr;
                        m_pPipeline = gst_parse_launch(pipelineDesc.c_str(), &error);

                        if (!m_pPipeline) {
                            std::cerr << "Failed to create pipeline: " << (error ? error->message : "unknown error") << "\n";
                            return;
                        }

                        m_pSource = gst_bin_get_by_name(GST_BIN(m_pPipeline), "source");
                        if (!m_pSource) {
                            std::cerr << "Failed to get appsrc element.\n";
                            gst_object_unref(m_pPipeline);
                            return;
                        }
#ifdef USE_NV12_COMPUTE_SHADER
                        GstCaps* caps = gst_caps_new_simple(
                            "video/x-raw",
                            "format", G_TYPE_STRING, "NV12",
                            "width", G_TYPE_INT, size.Width,
                            "height", G_TYPE_INT, size.Height,
                            "framerate", GST_TYPE_FRACTION, 60, 1,
                            nullptr); 
#else
                        GstCaps* caps = gst_caps_new_simple(
                            "video/x-raw",
                            "format", G_TYPE_STRING, "I420",
                            "width", G_TYPE_INT, size.Width,
                            "height", G_TYPE_INT, size.Height,
                            "framerate", GST_TYPE_FRACTION, 60, 1,
                            nullptr); 
#endif


                        g_object_set(G_OBJECT(m_pSource), "caps", caps, nullptr);
                        gst_caps_unref(caps);

                        g_object_set(G_OBJECT(m_pSource), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

                        ret = gst_element_set_state(m_pPipeline, GST_STATE_PLAYING);
                        if (ret == GST_STATE_CHANGE_FAILURE) {
                            std::cerr << "Failed to set pipeline to playing state.\n";
                            return;
                        }

                        m_pBus = gst_element_get_bus(m_pPipeline);
                        m_state = StreamingState::Streaming;
                    }, streamFrameSize);
            }
        }

        void GstService::StartUdpStream(winrt::Windows::Graphics::SizeInt32 streamFrameSize, std::string ipAddress, int port)
        {
            if (m_state == StreamingState::Ready)
            {
                frameSize = streamFrameSize;
                asyncTask = std::async(std::launch::async, [this](winrt::Windows::Graphics::SizeInt32 size, std::string address, int port)
                    {
                        GstStateChangeReturn ret;

                        std::ostringstream pipelineSs;

                        pipelineSs << "appsrc name=source is-live=true format=time ";
                        pipelineSs << "! queue leaky=downstream max-size-buffers=3 max-size-bytes=0 max-size-time=0 ";
                        pipelineSs << "! mfh264enc ";
                        pipelineSs << "bitrate=6000 rc-mode=3 quality-vs-speed=67 low-latency=true ";
                        pipelineSs << "cabac=false ";
                        pipelineSs << "! queue ";
                        pipelineSs << "! rtph264pay ";
                        pipelineSs << "! udpsink host=" << address << " port=" << port;

                        std::string pipelineDesc = pipelineSs.str();

                        GError* error = nullptr;
                        m_pPipeline = gst_parse_launch(pipelineDesc.c_str(), &error);

                        if (!m_pPipeline) {
                            std::cerr << "Failed to create pipeline: " << (error ? error->message : "unknown error") << "\n";
                            return;
                        }

                        m_pSource = gst_bin_get_by_name(GST_BIN(m_pPipeline), "source");
                        if (!m_pSource) {
                            std::cerr << "Failed to get appsrc element.\n";
                            gst_object_unref(m_pPipeline);
                            return;
                        }

#ifdef USE_NV12_COMPUTE_SHADER
                        GstCaps* caps = gst_caps_new_simple(
                            "video/x-raw",
                            "format", G_TYPE_STRING, "NV12",
                            "width", G_TYPE_INT, size.Width,
                            "height", G_TYPE_INT, size.Height,
                            "framerate", GST_TYPE_FRACTION, 60, 1,
                            nullptr);
#else
                        GstCaps* caps = gst_caps_new_simple(
                            "video/x-raw",
                            "format", G_TYPE_STRING, "I420",
                            "width", G_TYPE_INT, size.Width,
                            "height", G_TYPE_INT, size.Height,
                            "framerate", GST_TYPE_FRACTION, 60, 1,
                            nullptr);
#endif

                        g_object_set(G_OBJECT(m_pSource), "caps", caps, nullptr);
                        gst_caps_unref(caps);

                        g_object_set(G_OBJECT(m_pSource), "stream-type", 0, "format", GST_FORMAT_TIME, nullptr);

                        ret = gst_element_set_state(m_pPipeline, GST_STATE_PLAYING);
                        if (ret == GST_STATE_CHANGE_FAILURE) {
                            std::cerr << "Failed to set pipeline to playing state.\n";
                            return;
                        }

                        m_pBus = gst_element_get_bus(m_pPipeline);
                        m_state = StreamingState::Streaming;
                    }, streamFrameSize, ipAddress, port);
            }
        }

        void GstService::StopStream()
        {
            if (m_state == StreamingState::Streaming)
            {
                gst_element_set_state(m_pPipeline, GST_STATE_NULL);

                gst_message_unref(m_pMsg);
                gst_object_unref(m_pBus);
                gst_object_unref(m_pPipeline);

                m_state = StreamingState::Ready;
            }

            if (m_state != StreamingState::Ready)
            {
                m_state = StreamingState::Error;
            }
        }
    }
}