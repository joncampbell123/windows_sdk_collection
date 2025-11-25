
//
// Macros for building URB requests
//

#define UsbBuildInterruptOrBulkTransferRequest(urb, \
                                               length, \
                                               pipeHandle, \
                                               transferBuffer, \
                                               transferBufferMDL, \
                                               transferBufferLength, \
                                               transferFlags, \
                                               link) { \
            (urb)->UrbHeader.Function = URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER; \
            (urb)->UrbHeader.Length = (length); \
            (urb)->UrbBulkOrInterruptTransfer.PipeHandle = (pipeHandle); \
            (urb)->UrbBulkOrInterruptTransfer.TransferBufferLength = (transferBufferLength); \
            (urb)->UrbBulkOrInterruptTransfer.TransferBufferMDL = (transferBufferMDL); \
            (urb)->UrbBulkOrInterruptTransfer.TransferBuffer = (transferBuffer); \
            (urb)->UrbBulkOrInterruptTransfer.TransferFlags = (transferFlags); \
            (urb)->UrbBulkOrInterruptTransfer.UrbLink = (link); }
            

#define UsbBuildGetDescriptorRequest(urb, \
                                     length, \
                                     descriptorType, \
                                     index, \
                                     languageId, \
                                     transferBuffer, \
                                     transferBufferMDL, \
                                     transferBufferLength, \
                                     link) { \
            (urb)->UrbHeader.Function =  URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE; \
            (urb)->UrbHeader.Length = (length); \
            (urb)->UrbControlDescriptorRequest.TransferBufferLength = (transferBufferLength); \
            (urb)->UrbControlDescriptorRequest.TransferBufferMDL = (transferBufferMDL); \
            (urb)->UrbControlDescriptorRequest.TransferBuffer = (transferBuffer); \
            (urb)->UrbControlDescriptorRequest.DescriptorType = (descriptorType); \
            (urb)->UrbControlDescriptorRequest.Index = (index); \
            (urb)->UrbControlDescriptorRequest.LanguageId = (languageId); \
            (urb)->UrbControlDescriptorRequest.UrbLink = (link); }



                
#define UsbBuildSelectConfigurationRequest(urb, \
                                         length, \
                                         configurationDescriptor) { \
            (urb)->UrbHeader.Function =  URB_FUNCTION_SELECT_CONFIGURATION; \
            (urb)->UrbHeader.Length = (length); \
            (urb)->UrbSelectConfiguration.ConfigurationDescriptor = (configurationDescriptor);    }
                
#define UsbBuildSelectInterfaceRequest(urb, \
                                      length, \
                                      configurationHandle, \
                                      interfaceNumber, \
                                      alternateSetting) { \
            (urb)->UrbHeader.Function =  URB_FUNCTION_SELECT_INTERFACE; \
            (urb)->UrbHeader.Length = (length); \
            (urb)->UrbSelectInterface.Interface.AlternateSetting = (alternateSetting); \
            (urb)->UrbSelectInterface.Interface.InterfaceNumber = (interfaceNumber); \
            (urb)->UrbSelectInterface.ConfigurationHandle = (configurationHandle);    }


#define UsbBuildVendorRequest(urb, \
                              cmd, \
                              length, \
                              transferFlags, \
                              reservedbits, \
                              request, \
                              value, \
                              index, \
                              transferBuffer, \
                              transferBufferMDL, \
                              transferBufferLength, \
                              link) { \
            (urb)->UrbHeader.Function =  cmd; \
            (urb)->UrbHeader.Length = (length); \
            (urb)->UrbControlVendorClassRequest.TransferBufferLength = (transferBufferLength); \
            (urb)->UrbControlVendorClassRequest.TransferBufferMDL = (transferBufferMDL); \
            (urb)->UrbControlVendorClassRequest.TransferBuffer = (transferBuffer); \
            (urb)->UrbControlVendorClassRequest.RequestTypeReservedBits = (reservedbits); \
            (urb)->UrbControlVendorClassRequest.Request = (request); \
            (urb)->UrbControlVendorClassRequest.Value = (value); \
            (urb)->UrbControlVendorClassRequest.Index = (index); \
            (urb)->UrbControlVendorClassRequest.TransferFlags = (transferFlags); \
            (urb)->UrbControlVendorClassRequest.UrbLink = (link); }

//
// Get the USB status code
//

#define URB_STATUS(urb) ((urb)->UrbHeader.Status)
                
//
// Macros used for select interface and select configuration requests
//

//
// Calculates the size needed for a SELECT_CONFIGURATION URB request given
// the number of interfaces and the total number of pipes in all interfaces
// selected.
//

#define GET_SELECT_CONFIGURATION_REQUEST_SIZE(totalInterfaces, totalPipes) \
            (sizeof(struct _URB_SELECT_CONFIGURATION) + \
                ((totalInterfaces-1) * sizeof(USBD_INTERFACE_INFORMATION)) + \
                ((totalPipes)*sizeof(USBD_PIPE_INFORMATION)))

//
// Calculates the size needed for a SELECT_INTERFACE URB request given
// the number of pipes in int the alternate interface selected.
//

#define GET_SELECT_INTERFACE_REQUEST_SIZE(totalPipes) \
            (sizeof(struct _URB_SELECT_INTERFACE) + \
             ((totalPipes)*sizeof(USBD_PIPE_INFORMATION)))

//
// Calculates the size of the interface information structure needed to describe
// a give interface based on the number of endpoints.
//

#define GET_USBD_INTERFACE_SIZE(numEndpoints) (sizeof(USBD_INTERFACE_INFORMATION) + \
                        sizeof(USBD_PIPE_INFORMATION)*(numEndpoints))

//
// Calculates the size of an iso urb request given the number of packets
//

#define  GET_ISO_URB_SIZE(n) (sizeof(struct _URB_ISOCH_TRANSFER)+\
        sizeof(USBD_ISO_PACKET_DESCRIPTOR)*n)                          
