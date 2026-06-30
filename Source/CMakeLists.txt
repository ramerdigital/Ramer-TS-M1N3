#add_subdirectory(headless)

target_sources(ramer-ts-m1n3 PRIVATE
	myLookAndFeel.cpp
	myLookAndFeel.h
	PluginEditor.cpp
	PluginEditor.h
	PluginProcessor.cpp
	PluginProcessor.h
	RTNeuralLSTM.cpp
	RTNeuralLSTM.h
	R8BrainResampler.h
)

#target_precompile_headers(ramer-ts-m1n3 PRIVATE pch.h)
