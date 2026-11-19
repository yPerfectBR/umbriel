#pragma once

#include "config/config.h"

#include <functional>
#include <memory>
#include <string_view>

struct wl_display;
struct wlr_backend;
struct wlr_renderer;
struct wlr_session;

namespace umbriel {

  // Owns backend selection, hotplug handling, and the GPU exclusions. PCI-address exclusions of
  // secondary GPUs can change at runtime; path selectors and the primary GPU are fixed at startup.
  class BackendManager {
  public:
    [[nodiscard]] static std::unique_ptr<BackendManager> create(wl_display* display, const Config::Drm& config);
    ~BackendManager();

    BackendManager(const BackendManager&) = delete;
    BackendManager& operator=(const BackendManager&) = delete;

    wlr_backend* backend() const;
    wlr_session* session() const;
    [[nodiscard]] wlr_renderer* createRenderer();
    [[nodiscard]] bool verifyOpenDevices(std::string_view context);
    void markStarted();
    // Applies a changed [drm] section live: secondary GPUs that are no longer ignored are opened
    // and their outputs appear; newly ignored secondary GPUs are released after `release` ran for
    // each of their backends (to turn their outputs off). Returns false when the change needs a
    // restart (path selectors changed, the primary GPU would be excluded, or no native session).
    [[nodiscard]] bool applyDrmConfig(const Config::Drm& config, const std::function<void(wlr_backend*)>& release);
    void setDrmBackendRemovedHandler(std::function<void(const wlr_backend*)> handler);

  private:
    class Impl;
    explicit BackendManager(std::unique_ptr<Impl> impl);

    std::unique_ptr<Impl> m_impl;
  };

} // namespace umbriel
