module ESP32WIFI
  puts "WiFi Version....01"
  class WiFi
    def initialize 
      init_wifi

    end

    def wifi_connect ssid,password
      connect ssid,password
      #w.wifi_connect "TP-Link_6EA8_5G","22429735"
    end

    def wifi_disconnect 
      disconnect
    end

  end
end
