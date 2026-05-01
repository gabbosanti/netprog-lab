package main

import (
	"context"
	"flag"

	"github.com/antoninbas/p4runtime-go-client/pkg/p4switch"
	"github.com/antoninbas/p4runtime-go-client/pkg/server"

	log "github.com/sirupsen/logrus"
)

func main() {

	// Inizializza variabili "flag" che vengono passate come argomento
	var deviceId int
	flag.IntVar(&deviceId, "id", 1, "DeviceId")
	var cntPort int
	flag.IntVar(&cntPort, "cp", 3333, "cntPort")
	var swAddr string
	flag.StringVar(&swAddr, "sw", "100.0.1.1:50050", "swAddr")
	var verbose bool
	flag.BoolVar(&verbose, "verbose", false, "Enable verbose mode with debug log messages")
	var trace bool
	flag.BoolVar(&trace, "trace", false, "Enable trace mode with log messages")
	var configName string
	flag.StringVar(&configName, "config", "/config/config.json", "Program name")
	var topologyName string
	flag.StringVar(&topologyName, "topology", "", "Topology name")
	var certFile string
	flag.StringVar(&certFile, "cert-file", "", "Certificate file for tls")
	flag.Parse()

	if verbose {
		log.SetLevel(log.DebugLevel)
	}
	if trace {
		log.SetLevel(log.TraceLevel)
	}

	ctx, cancel := context.WithCancel(context.Background())

	sw := p4switch.CreateSwitch(uint64(deviceId), configName, 3, certFile, swAddr)

	if err := sw.RunSwitch(ctx); err != nil {
		sw.GetLogger().Errorf("Cannot start")
		log.Errorf("%v", err)
	}

	if sw == nil {
		log.Info("Switch not started !")
		return
	}

	server.StartServer(sw, topologyName, ctx, cntPort)
	cancel()
}
