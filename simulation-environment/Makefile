MAKE_DIR = $(PWD)

CONFIGURATION_SERVER_DIR := $(MAKE_DIR)/src/configuration_server
SIMULATION_MODEL_DIR := $(MAKE_DIR)/src/simulation_model
EVENT_QUEUE_DIR := $(MAKE_DIR)/src/queues/event_queue_1
MODEL_1_DIR := $(MAKE_DIR)/src/subcomponents/model_1
MODEL_2_DIR := $(MAKE_DIR)/src/subcomponents/model_2

all:
	@$(MAKE) -C $(CONFIGURATION_SERVER_DIR) -f Makefile
	@$(MAKE) -C $(SIMULATION_MODEL_DIR) -f Makefile
	@$(MAKE) -C $(EVENT_QUEUE_DIR) -f Makefile
	@$(MAKE) -C $(MODEL_1_DIR) -f Makefile
	@$(MAKE) -C $(MODEL_2_DIR) -f Makefile
	
configuration-server:
	@$(MAKE) -C $(CONFIGURATION_SERVER_DIR) -f Makefile
	
simulation-model:
	@$(MAKE) -C $(SIMULATION_MODEL_DIR) -f Makefile
	
event-queue:
	@$(MAKE) -C $(EVENT_QUEUE_DIR) -f Makefile
	
model1:
	@$(MAKE) -C $(MODEL_1_DIR) -f Makefile
	
model2:
	@$(MAKE) -C $(MODEL_2_DIR) -f Makefile
	
clean:
	@$(MAKE) -C $(CONFIGURATION_SERVER_DIR) -f Makefile clean
	@$(MAKE) -C $(SIMULATION_MODEL_DIR) -f Makefile clean
	@$(MAKE) -C $(EVENT_QUEUE_DIR) -f Makefile clean
	@$(MAKE) -C $(MODEL_1_DIR) -f Makefile clean
	@$(MAKE) -C $(MODEL_2_DIR) -f Makefile clean
	
distclean:
	@$(MAKE) -C $(CONFIGURATION_SERVER_DIR) -f Makefile distclean
	@$(MAKE) -C $(SIMULATION_MODEL_DIR) -f Makefile distclean
	@$(MAKE) -C $(EVENT_QUEUE_DIR) -f Makefile distclean
	@$(MAKE) -C $(MODEL_1_DIR) -f Makefile distclean
	@$(MAKE) -C $(MODEL_2_DIR) -f Makefile distclean